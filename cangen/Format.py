from dataclasses import dataclass

@dataclass
class Format:
   '''
   Represents a format to be applied to data after decoding.
   '''
   repr: str = ""

@dataclass
class Temperature(Format):
      repr: str = "temperature"

@dataclass
class LowVoltage(Format):
      repr: str = "low_voltage"

@dataclass
class Torque(Format):
      repr: str = "torque"

@dataclass
class HighVoltage(Format):
      repr: str = "high_voltage" 

@dataclass
class Current(Format):
      repr: str = "current"

@dataclass
class Angle(Format):
      repr: str = "angle"

@dataclass
class AngularVelocity(Format):
      repr: str = "angular_velocity"

@dataclass
class Frequency(Format):
      repr: str = "frequency"

@dataclass
class Power(Format):
      repr: str = "power"

@dataclass
class Timer(Format):
      repr: str = "timer"

@dataclass
class Flux(Format):
      repr: str = "flux"

@dataclass
class CellVoltage(Format):
      repr: str = "cell_voltage"
