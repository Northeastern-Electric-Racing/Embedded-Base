# CANGEN
> Definition of all CAN messages to be sent across the car with [Calypso]() to generate code to encode, decode, and simulate

## Adding Messages

To have a CAN message recognized by Calypso, you will need to add to the existing JSON spec files. If you are beginning a new project, create a new .json file for that project. Otherwise, add to one of the existing files.

Each file is comprised of a list of CAN Messages, which hold their own lists of NetFields and CANPoints. CANPoints are raw bits that are sent with a CAN Message, and NetFields correlate CANPoints to names and units so that they can be made sense of in Argos or MQTTUI.


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
        "values": []
      },
      {
        ...
      }
    ],
    "points": [
      {
        "size": 8,
      },
      {
        ...
      }
    ],
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
3. CANPoint objects (in a list within a Message object)


#### Message

At the root of any spec-compliant JSON file is a list of Message objects. Each Message object has the following members:
- `id`, a hexadecimal string of the CAN ID (extended CAN supported and default)
- `desc`, string representing a quick descroption of the CAN message
- `points`, a list of CAN Points being sent from the Message
- `fields`, a list of MQTT messages being sent from the Message

Occassionally you may want Calypso to also send a message on the CAN network. Use the above fields, with these modifications/additions to treat the Message as an Encodable Message:
- `key`, string representing the key to index the Encodable Message to, e.g. it would be sent to Calypso on `"Calypso/Bidir/Command/{key}"` 
- `is_ext`, boolean representing whether the CAN ID is extended or standard (`false` by default)
It is recommended that the decoding of the message be done to the topic `"Calypso/Bidir/State/{key}/{field_name}"`. Note decoding works exactly the same with these messages, so serves as an accurate representation of what Calypso is current sending out to the car.
- `bidir_mode`, string specifying which bidirectionality functional profile the message uses (`broadcast` by default). Options:
  - `broadcast`: The message is sent on a loop of about one second with the most recent value, or the default value (see below).  Default value is required.
  - `oneshot`: The message is sent once instantaneously upon reception of a setting.  There are no default values used. 

You may want to publish the fields of a message to additional MQTT clients. Add the following optional field to specify which additional client ports the fields should be published on.
- `clients`, (optional) list of u16s representing the ports of additional MQTT clients to publish to. Currently supported additional ports are: 1882

#### NetField

Within the `fields` member of a Message object, there is a list of NetField objects. Think 1 MQTT Topic per NetField. Each NetField has the following members:
- `name`, string of the message topic name, e.g. `The/Topic/Name`. Going 3 levels of slashes is usually preferred, don't put trailing slashes.
  - To embed live Point values in `name`, include point indices (from 1) in the name. Ex. `"Hello/{1}/World/{2}/Status"`
- `unit`, string of the unit of the data, e.g. `mph`
- `values`, a list of correlated Points to be sent with the Field. **Note that Points are indexed by 1, and out-of-bounds indices will cause build failure**


#### Point

Within the `points` member of a NetField object, there is a list of Point objects. A Point object represents one set of bits in a CAN message. This separates CAN decoding logic from MQTT encoding information. Each Point has the following members:
- `size`, an integer representing the size to be read in bits
- `parse` (optional), boolean representing whether or not this field should be parsed (if `false`, it is literally skipped). Best used for byte-alignment padding
- `signed` (optional), boolean representing whether or not the number is signed in two's complement form (`false` by default) 
- `endianness` (optional), string representing the byte endianness of the bits being read, either `"big"` or `"little"` (`"big"` by default)
- `format` (optional), string representing the format of the bits (e.g. `divide100`) (blank by default)
  - Use an existing formatter if possible.  To create a new one, add it to the `impl FormatData` block in `Calypso/src/data.rs`.  Name it what it does if it could be reused by unrelated functions (ex. divide by 100 --> `divide100`) or if its very obscure use whats its used for (ex. multiply by 0029 in IMU datasheet --> `acceleration`). Include `_d` and `_e` implementations for decoding and encoding, respectively
- `default_value` (optional, only for Encodable Messages), float representing the default value to be sent before a command is received or when an empty command is received. This is ignored when decoding the Point, and ignored if using bidirectionality profile `oneshot` (`0` by default) 
- `ieee754_f32` (optional), boolean indicating if the bits in the Point should be interpreted as an IEEE754 32-bit float. **Be sure to endian swap the float before sending!!** (`false` by default)
- `sim` (optional), the simulation mode for the Point. See below

#### Sim
Within the `sim` member of a CANPoint object, there is a single sim object.  This object is one of two types, either `sweep` or `enum`.  However these types are implied, not written in the JSON, just use them correctly!

`sweep`: sweep a certain range of values
- `min`, float, the minium value to emit
- `max`, float, the maximum value to emit
- `inc_min`, float, the minimum increment
- `inc_max`, float, the maximum increment
- `round`, bool, whether to round values to a whole number (more precise rounding is not supported) (default: false)

**or**  
`enum`: choose from a certain list of values
- `options`, list[[`value`, `freq`]...], a list containing lists of length two.  Each inner list's first item is the value to be emitted, and the second item is the probability the item should be emitted.  **These probabilities must add to 1, or they won't be obeyed.**

## Spec Rules

**Many of the above requirements will be enforced on-push for Embedded-Base. This will make bad spec fail to build!**

Messages should follow these rules:
- Descriptions should contain only letters and whitespace (`_` is allowed)
- Message totals should be byte aligned, meaning that the total number of bits in a Message should be a power of 2. If your message doesn't add up to this as you've written it, use a padding CANPoint, and specify the `parse` field as `false` 
- Wherever possible, bit-wise decoding and byte-wise decoding should happen in seperate bytes to avoid confusion. Ex. If there are 5 messages of size one (booleans), add a 3 bit filler before adding a 16 bit number
- Make the topic of an Encodable CAN Message be `"Calypso/Bidir/State/{key}/{field_name}"`

Fields should follow these rules:
- To correlate CAN Points with a NetField, specify the indices of the points (**indexed from one (1)**) in the Field's `values` member
- To emplace the value of a CAN Point in the `name` of a NetField at runtime (i.e. in-topic naming), enclose the relevant point in curly braces in the name. Ex. `"name": "Hello/{1}/World/{2}/Status"`
- The Points referenced by the `values` member should not be out-of-bounds. Ex. if the Message contains 3 points, the only valid elements in `values` would be 1, 2, or 3
- The above also goes for possible indices in the `name` field  

Points should follow these rules:
- Most significant bit should be the leftmost bit in each byte of data
- Points of less than 8 bits' endianness should not be specified and will not do anything
- **Signed CANPoints must be 8,16,or 32 bits and byte aligned!**
- **Little endian messages must be 8,16, or 32 bits and byte aligned!**
- Maximum size of a sent message (default, aka `"parse": true`), is 32 bits
- Unsent points should only contain the `size` parameter
- Sim enum frequencies must add up to 1 or they will not be respected.
- IEEE754 Float Points must be 32 bits 

In general, use previous examples for most things.

**Note:** Single bit messages are memcpy-ed wrong by default, you may need to use `reverse_bits` in `c_utils.h`
**Note:** Please use big endian whenever possible, as that is the standard at which our MC, Charger Box, etc. expect it.  Use `endian_swap` in `c_utils.h`
