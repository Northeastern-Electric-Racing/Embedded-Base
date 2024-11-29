# CANGEN
> Definition of all CAN messages to be sent across the car with Python scripts to generate code to encode and decode

Here is a custom Python module built to generate embedded code for encoding and decoding CAN messages. The end of this module is nigh.

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
9. Make the topic of an EncodableCANMsg be `"Calypso/Bidir/State/{key}/{field_name}"`
10. The description field must be only letters and spaces, upper or lowercase
11. Sim enum frrquencies must add up to 1 or they will not be respected.

Message guide:
1. Use previous examples for most things
2. Use an existing formatter if possible.  To create a new one, add it to Format.py and RustSynth.py.  Name it what it does if it could be reused by unrelated functins (ex. divide by 100 --> divide100) or if its very obscure use whats its used for (ex. multiply by 0029 in IMU datasheet --> acceleration).

Note: Single bit messages are memcpy-ed wrong by default, you may need to use `reverse_bits` in `c_utils.h`
Note: Please use big endian whenever possible, as that is the standard at which our MC, Charger Box, etc. expect it.  Use `endian_swap` in `c_utils.h`


## JSON Spec.


### Decodable Messages
For example:
```json 
[
  {
    "id": "0x80",
    "desc": "A quick description goes here",
    "fields": [
      {
        "name": "The/Topic/Name",
        "unit": "unit_here",
        "points": [
          {
            "size": 8,
          }
          ,
          {
            ...
          }
        ]
      }
      ,
      {
        ...
      }
    ]
  }
  ,
  {
    ...
  }
]
```


### Structure 

JSON files should be structured according to a strict hierarchy of:
1. Message objects (in a list)
2. NetField objects (in a list within a Message object)
3. CANPoint objects (in a list within a NetField object)


#### Message

At the root of any spec-compliant JSON file is a list of Message objects. Each Message object has the following members:
- `id`, a hexadecimal string of the CAN ID (extended CAN supported and default)
- `desc`, string representing a quick descroption of the CAN message
- `fields`, a list of MQTT messages being sent from the Message

Occassionally you may want Calypso to also send a message on the CAN network. Use the above fields, with these modifications/additions to treat the Message as an Encodable Message:
- `key`, string representing the key to index the Encodable Message to, e.g. it would be sent to Calypso on `"Calypso/Bidir/Command/{key}"` 
- `is_ext`, boolean representing whether the CAN ID is extended or standard (`false` by default)
It is recommended that the decoding of the message be done to the topic `"Calypso/Bidir/State/{key}/{field_name}"`. Note decoding works exactly the same with these messages, so serves as an accurate representation of what Calypso is current sending out to the car.

Also, you should allow for simulation of the data point whenever possible.
  - `sim_freq`, integer frequency of which this message is usually emitted, in ms



#### NetField

Within the `fields` member of a Message object, there is a list of NetField objects. Think 1 MQTT Topic per NetField. Each NetField has the following members:
- `name`, string of the message topic name, e.g. `The/Topic/Name`. Going 3 levels of slashes is usually preferred, don't put trailing slashes.
- `unit`, string of the unit of the data, e.g. `mph`
- `points`, a list of Point objects to be used for this MQTT message
- `send` (optional), boolean representing whether this message should be send over the network (`true` by default)
- `topic_append` (optional), boolean representing whether to append the topic name with the value of the first CANPoint in `points` (`false` by default)
- `sim` (optional) a sim object to define simulation parameters


#### Point

Within the `points` member of a NetField object, there is a list of Point objects. A Point object represents one set of bits in a CAN message. This separates CAN decoding logic from MQTT encoding information. Each Point has the following members:
- `size`, an integer representing the size to be read in bits
- `signed` (optional), boolean representing whether or not the number is signed in two's complement form (`false` by default) 
- `endianness` (optional), string representing the byte endianness of the bits being read, either `"big"` or `"little"` (`"big"` by default)
- `format` (optional, not recommended), string representing the final type of the data (`"f32"` by default)
- `default_value` (optional, only for Encodable Messages), float representing the default value to be sent before a command is received or when an empty command is received. This is ignored when decoding the Point (`0` by default) 

#### Sim
Within the `sim` member of a NetField object, there is a single sim object.  This object is one of two types, either `sweep` or `enum`.  However these types are implied, not written in the JSON, just use them correctly!

`sweep`: sweep a certain range of values
- `min`, float, the minium value to emit
- `max`, float, the maximum value to emit
- `inc_min`, float, the minimum increment
- `inc_max`, float, the maximum increment
- `round`, bool, whether to round values to a whole number (more precise rounding is not supported)

or  
`enum`: choose from a certain list of values
- `options`, list[[`value`, `freq`]...], a list containing lists of length two.  Each inner list's first item is the value to be emitted, and the second item is the probability the item should be emitted.  **These probabilities must add to 1, or they won't be obeyed.**


### Directory Structure
```
│
├── can-messages/
│   ├── .json files defining decode/encode specs 
│   ├── ...
│   └── ...
│
├── CANField.py:
│   ├── NetField            # Class which describes the topic and unit of one or more can points
│   └── CANPoint            # Class which describes the decoding operations done to bits of a can message
│
├── Format.py:
│   ├── Format              # Represents a format to apply to data after decoding 
│   ├── Divide10            # Format indicating decoded data should be divided by 10 
│   ├── Divide100           # Format indicating decoded data should be divided by 100 
│   ├── Divide1000          # Format indicating decoded data should be divided by 10000
│   └── Acceleration        # Format indicating decoded data should be handled as acceleration (* or / 0.0029) 
│
├── CANMsg.py:
│   └── CANMsg              # Represents a full CAN message
│
├── Messages.py:
│   └── Messages            # Container for all messages related to a node
│
├── YAMLParser.py:
│   └── YAMLParser          # Parses a YAML file into CAN message structures
│
├── RustSynth.py:
│   └── RustSynth           # Generates Rust code from a YAML CAN messages structure
│
├── RustSynthFromJSON.py:
│   └── RustSynthFromJSON   # Generates Rust code from a JSON CAN messages structure
│
├── jsongen:                # Python script to convert YAML files into equivalent JSON
│
└── README.md               # This file!
```
