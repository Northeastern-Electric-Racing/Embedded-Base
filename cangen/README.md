# CANGEN
> Definition of all CAN messages to be sent across the car with Python scripts to generate code to encode and decode

Here is a custom Python module built to generate embedded code for encoding and decoding CAN messages.

## Adding Messages

Messages should follow these rules:
1. Messages must be MSB leftmost bit
2. Messages <=8 bits endianness should be specified
3. Wherever possible, bit-wise decoding and byte-wise decoding should happen in seperate bytes to avoid confusion.
Ex. If there are 5 messages of size one (booleans), add a 3 bit filler before adding a 16 bit number
4. Message totals should be byte aligned, meaning total bit size of a message should be a power of 8

Message guide:
1. Use previous examples for most things
2. Use an existing formatter if possible.  To create a new one, add it to Format.py and RustSynth.py.  Name it what it does if it could be reused by unrelated functins (ex. divide by 100 --> divide100) or if its very obscure use whats its used for (ex. multiply by 0029 in IMU datasheet --> acceleration).

Note: Single bit messages are memcpy-ed wrong by default, you may need to use `reverse_bits` in `c_utils.h`
Note: Please use big endian whenever possible, as that is the standard at which our MC, Charger Box, etc. expect it.  Use `endian_swap` in `c_utils.h`

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
