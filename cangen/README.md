# CANGEN
> Definition of all CAN messages to be sent across the car with Python scripts to generate code to encode and decode

Here is a custom Python module built to generate embedded code for encoding and decoding CAN messages.

### Directory Structure
```
|
|───CANField.py:
|   |───DiscreteField   # Single data point to stand alone
|   └───CompositeField  # Group of related data points that won't make sense alone
|
|───CANMsg.py:
|   └───CANMsg          # Represents a full CAN message
|
|───Messages.py:
|   └───Messages        # Container for all messages related to a node
|
|───Decoding.py:
|   |───LittleEndian
|   |───BigEndian
|   └───TwosComplement
|
|───YAMLParser.py:
|   └───YAMLParser      # Parses a YAML file into CAN message structures
|
|───RustSynth.py:
|   └───RustSynth       # Generates Rust code from a CAN messages structure
|
|───CSynth.py:
|   └───CSynth          # Generates  code from a CAN messages structure
|
└───README.md           # This file!
