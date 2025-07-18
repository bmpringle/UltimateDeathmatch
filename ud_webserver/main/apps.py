from django.apps import AppConfig
from django.db.backends.signals import connection_created
from django.dispatch import receiver

@receiver(connection_created)
def func(sender, **kwargs):
	from main.models import Game, GAME_CONNECTION_STATE

	for game in Game.objects.all():
		game.server_connected = GAME_CONNECTION_STATE.UNKNOWN
		game.save()

	games_not_setup = True
	while games_not_setup:
		games_not_setup = False
		for game in Game.objects.all():
			if game.server_connected == GAME_CONNECTION_STATE.UNKNOWN:
				print(f"Try to establish a connection with a game of id #{game.id}")
				game.establish_connection()
				games_not_setup = True
			elif game.server_connected == GAME_CONNECTION_STATE.DISCONNECTED:
				print(f"Try to start a server for a game of id #{game.id}")
				game.establish_connection()
				games_not_setup = True

class MainConfig(AppConfig):
	default_auto_field = 'django.db.models.BigAutoField'
	name = 'main'
