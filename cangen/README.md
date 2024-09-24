# CANGEN
> Definition of all CAN messages to be sent across the car with Python scripts to generate code to encode and decode

Here is a custom Python module built to generate embedded code for encoding and decoding CAN messages.

## Adding Messages

Messages should follow these rules:
1. Most significant bit should be the leftmost bit in each byte of data
2. Messages <=8 bits endianness should not be specified and will not do anything
3. Wherever possible, bit-wise decoding and byte-wise decoding should happen in seperate bytes to avoid confusion.
Ex. If there are 5 messages of size one (booleans), add a 3 bit filler before adding a 16 bit number
4. Message totals should be byte aligned, meaning total bit size of a message should be a power of 2
5. **Signed messages must be 8,16,or 32 bits and byte aligned!**
6. **Little endian messages must be 8,16, or 32 bits and byte aligned!**
7. Maximum size of a sent message (default, aka send=True), is 32 bits
8. Unsent messages should only contain the size parameter
9. Make the topic of an EncodableCANMsg be "Calypso/Bidir/State/{key}/{field_name}"
10. The description field must be only letters and spaces, upper or lowercase

Message guide:
1. Use previous examples for most things
2. Use an existing formatter if possible.  To create a new one, add it to Format.py and RustSynth.py.  Name it what it does if it could be reused by unrelated functins (ex. divide by 100 --> divide100) or if its very obscure use whats its used for (ex. multiply by 0029 in IMU datasheet --> acceleration).

Note: Single bit messages are memcpy-ed wrong by default, you may need to use `reverse_bits` in `c_utils.h`
Note: Please use big endian whenever possible, as that is the standard at which our MC, Charger Box, etc. expect it.  Use `endian_swap` in `c_utils.h`

## YAML Spec.

### Decodable messages 

```
# all files start with this yaml
!Messages
msgs: 
- !CANMsg
    id: "0x80" # hexadecimal string of the CAN ID (extended CAN supported)
    desc: "a quick description goes here"
    fields: # list of MQTT messages being sent from this CANMsg
    - !NetField
        name: "The/Topic/Name" # the message topic name, going 3 levels of slashes is usally prefered, dont put trailing slashes
        unit: "unit_here" # the unit of the data, ex. mph
        send: true # (optional) whether this message should be sent over the network, default is true
        topic_append: false # (optional) whether to append the topic name with the value of the first CANPoint in points
        points: # list of CAN bits to be used for this message
        - !CANPoint:
            size: 8 # the integer size to be read, in bits
            signed: false # (optional) whether the number is in signed twos complement form, default is false
            endianness: "big" # (optional) the byte endianness of the bits being read, "big" or "false", "big" is default
            format: "formatter_name" # (optional) the name of the formatter to use, default is no formatting, see above for formatter info
            final_type: "f32" # (optional, not recommended) the final type of the data           

```

### Encodable messages
Occassionally you may want Calypso to also send a message on the CAN network.  Use the above fields, with these modifications/additions:
It is recommended that the decoding of the message be done to the topic "Calypso/Bidir/State/{key}/{field_name}".  Note decoding works exactly the same with these messages, so serves as an accurate representation of what Calypso is current sending out to the car.

```
- !EncodableCANMsg # use this instead of CANMsg
    is_ext: false # (optional) whether the CAN ID of the message is extended or standard, default is false
    key: # the key to index the encodable message to, so it would be sent to Calypso on "Calypso/Bidir/Command/{key}"


        - !CANPoint:
            default: 0 # (optional) the default value to be sent before a command is recieved or when an empty command is recieved, default is 0.  This is ignored when decoding the point
```

### Directory Structure
```
│
├── CANField.py:
│   ├── NetField # a class which describes the topic and unit of one or more can points
│   └── CANPoint # a class which describes the decoding operations done to bits of a can message
│
├── CANMsg.py:
│   └── CANMsg          # Represents a full CAN message
│
├── Messages.py:
│   └── Messages        # Container for all messages related to a node
│
├── YAMLParser.py:
│   └── YAMLParser      # Parses a YAML file into CAN message structures
│
├── RustSynth.py:
│   └── RustSynth       # Generates Rust code from a CAN messages structure
│
├── CSynth.py:
│   └── CSynth          # Generates code from a CAN messages structure
│
└── README.md           # This file!
```
