from django.db import models
from django.db.models.fields import IntegerField, CharField
from django.contrib.auth import get_user_model
from django.contrib.auth.hashers import PBKDF2PasswordHasher
from django.db.models.signals import post_save
from ud_matchserver.server import start
from django.dispatch import receiver
from multiprocessing import Process
import random
from twisted.internet.protocol import Protocol, ClientFactory, connectionDone
from twisted.internet import reactor

class GAME_CONNECTION_STATE:
	UNKNOWN = 1
	CONNECTING = 2
	CONNECTED = 3
	DISCONNECTED = 4

	@classmethod
	def choices(cls):
		return [(1, "Unknown"), (2, "Connecting"), (3, "Connected"), (4, "Disconnected")]



class InternalCommunicationProtocolWebserver(Protocol):
	def __init__(self, game_id):
		self.game_id = game_id

	def connectionMade(self):
		game = Game.objects.get(id=self.game_id)
		game.server_connected = GAME_CONNECTION_STATE.CONNECTED
		game.save()

	def dataReceived(self, data):
		pass

	def connectionLost(self, reason=connectionDone):
		game = Game.objects.get(id=self.game_id)
		game.server_connected = GAME_CONNECTION_STATE.DISCONNECTED
		game.save()
		game.establish_connection()

class InternalCommuncationFactoryWebserver(ClientFactory):
	def __init__(self, game_id):
		self.game_id = game_id

	def startedConnecting(self, connector):
		game = Game.objects.get(id=self.game_id)
		game.server_connected = GAME_CONNECTION_STATE.CONNECTING
		game.save()

	def buildProtocol(self, addr):
		return InternalCommunicationProtocolWebserver(self.game_id)

	def clientConnectionFailed(self, connector, reason):
		game = Game.objects.get(id=self.game_id)
		game.server_connected = GAME_CONNECTION_STATE.DISCONNECTED
		game.save()
		game.establish_connection()

	def clientConnectionLost(self, connector, reason):
		game = Game.objects.get(id=self.game_id)
		game.server_connected = GAME_CONNECTION_STATE.DISCONNECTED
		game.save()
		game.establish_connection()

class Game(models.Model):
	hasher = PBKDF2PasswordHasher()

	gameserver_port = IntegerField(blank=True, unique=True, default=-1)
	internal_comms_port = IntegerField(blank=True, unique=True, default=-1)

	name = CharField()
	password = CharField(blank=True)
	player_ids = CharField(blank=True)

	server_connected = IntegerField(default=GAME_CONNECTION_STATE.DISCONNECTED, choices=GAME_CONNECTION_STATE.choices())

	@classmethod
	def encode_password(cls, pwd):
		return cls.hasher.encode(pwd, cls.hasher.salt(), cls.hasher.iterations)

	@classmethod
	def check_password(cls, game, pwd):
		decoded_pass = cls.hasher.decode(game.password)
		encoded_pwd = cls.hasher.encode(pwd, decoded_pass['salt'], decoded_pass['iterations'])
		return encoded_pwd == game.password

	def add_player(self, user):
		self.player_ids += "," + user.pk
		self.save();

	def remove_player(self, user):
		self.player_ids = str(self.player_ids).replace(user.pk, "").replace(",,", ",")
		self.save();

	def get_players(self):
		return get_user_model().objects.filter(id__in=str(self.player_ids).split(","))

	def _set_server_ports(self):
		try:
			self.gameserver_port = random.randint(10_000, 11_000)
			self.internal_comms_port = random.randint(9_000, 9_999)
			self.save()
		except:
			self._set_server_ports()

	def establish_connection(self):
		if self.internal_comms_port == -1 or self.gameserver_port == -1:
			self._set_server_ports()
		if self.server_connected == GAME_CONNECTION_STATE.DISCONNECTED:
			Process(target=start, name=self.name, kwargs={"gs_port": self.gameserver_port, "ic_port": self.internal_comms_port, "webserver_addr": "127.0.0.1", "game_id": self.id}).start()
		if self.server_connected in [GAME_CONNECTION_STATE.DISCONNECTED, GAME_CONNECTION_STATE.UNKNOWN]:
			reactor.connectTCP("127.0.0.1", self.internal_comms_port, InternalCommuncationFactoryWebserver(self.id))

	def has_password(self):
		return self.password != ""


@receiver(post_save, sender=Game)
def run_after_game_creation(sender, instance, created, **kwargs):
	if created:
		instance.establish_connection()
