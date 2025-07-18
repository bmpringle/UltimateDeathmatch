from typing import Any
from twisted.internet import reactor
from twisted.internet.protocol import DatagramProtocol
import struct

class Data:
	type : Any
	size : int
	unpack_str : str
	default_value : Any

	def __init__(self, default_value=None, **kwargs):
		self.default_value = default_value
		self.length = kwargs.get("length", 1)
		if self.type is None or self.unpack_str is None or self.size is None:
			raise Exception("Must instantiate type, size and unpack_str in class derived from Data.")
		if self.default_value is not None and not isinstance(self.default_value, self.type):
			raise Exception("Data's default value must be of the instance of the data's type if it is set.")

	def decode(self):
		return self.type(struct.unpack(self.unpack_str, self.size * self.length))

class FloatData(Data):
	type = float
	size = 4
	unpack_str = "f<"

class StringData(Data):
	type = str
	size = 1
	unpack_str = "c<"

	def __init__(self, default_value=None):
		super().__init__(default_value)

class DecodedDatagramMetaclass(type):
	def __new__(cls, name, bases, attrs):
		data_attrs = []
		for attr in attrs:
			if isinstance(attrs[attr], Data):
				data_attrs.append((attr, attrs[attr].type, attr))
				attrs[attr] = attrs[attr].default_value
		sorted(data_attrs, key=lambda d:d[2])
		i = 0
		while i < len(data_attrs):
			data_attr = data_attrs[i]
			for j in range(i + 1, len(data_attrs)):
				if data_attrs[j][2].startswith(data_attr[2]):
					data_attrs[j][2] = "%" + data_attrs[j][2]
					i = -1
					sorted(data_attrs, key=lambda d:d[2])
					break
			i += 1
		attrs["data_ids"] = {}
		for data_attr in data_attrs:
			attrs["data_ids"][data_attr[2]] = data_attr[0]
			attrs[data_attr[0] + "_type"] = data_attr[1]
		return super().__new__(cls, name, bases, attrs)

class DecodedDatagram(metaclass=DecodedDatagramMetaclass):
	def decode(self, datagram):
		buffer = ""
		while len(datagram) > 0:
			buffer += datagram[0].decode("ascii")
			datagram = datagram[1:]
			if buffer in getattr(self, "data_ids"):
				attr = getattr(self, "data_ids")[buffer]
				attr_type = getattr(self, attr + "_type")
				setattr(self, attr, attr_type.decode())
				buffer = ""

class TimedDatagram(DecodedDatagram):
	timestamp = FloatData()
	pos_x = FloatData()
	pos_y = FloatData()
	pos_z = FloatData()

class MatchServer:
	def recieved_datagram(self, addr, datagram : DecodedDatagram, transport):
		print(f"Recieved datagram {datagram} from {addr}")

	def run(self):
		while True:
			pass

class TimedDatagramProtocol(DatagramProtocol):
	most_recent_dg_times = {}
	time_size = 3 # in bytes
	match_server : MatchServer

	def __init__(self, server):
		self.match_server = server

	def datagramReceived(self, datagram: bytes, addr) -> None:
		decoded_datagram = TimedDatagram()
		decoded_datagram.decode(datagram)
		self.match_server.recieved_datagram(addr, decoded_datagram, self.transport)
		super().datagramReceived(datagram, addr)

from twisted.internet.protocol import Protocol, Factory

class InternalCommunicationProtocolServer(Protocol):
	def connectionMade(self):
		print("Established connection to webserver!")

	def dataReceived(self, data):
		pass

class InternalCommunicationFactoryServer(Factory):
	def buildProtocol(self, addr):
		return InternalCommunicationProtocolServer()

def start(**kwargs):
	if "gc_port" not in kwargs or "ic_port" not in kwargs or "webserver_addr" not in kwargs or "game_id" not in kwargs:
		raise Exception("Missing required kwargs gc_port, webserver, game_id, or ic_port to launch match server.")
	print(f'Starting a match server on game port {kwargs.get("gc_port")} and internal comms port {kwargs.get("ic_port")} with the related webserver at {kwargs.get("webserver_addr")} for the game {kwargs.get("game_id")}.')
	match_server = MatchServer()
	reactor.listenUDP(kwargs.get("gc_port"), TimedDatagramProtocol(match_server))
	reactor.listenTCP(kwargs.get("ic_port"), InternalCommunicationFactoryServer())
	match_server.run()
