from cangen.Result import Result
from typing import List, Dict, Optional
import math

class RustSynthFromJSON:
	"""
	A class to synthesize Rust from a given CANMsg spec (in JSON).
	"""

	def parse_messages(self, msgs: List[Dict]) -> Result:
		"""
		The main function of the RustSynth class. Takes a list of Dicts(i.e. CANMsgs) and
		returns a Result object that contains the synthesized Rust code for the
		decode_data.rs and master_mapping.rs files
		"""
		result = Result("", "", "", "", "")
		result.decode_data += RustSnippets.ignore_clippy
		result.decode_data += RustSnippets.decode_data_import
		result.decode_data += RustSnippets.decode_mock

		result.encode_data += RustSnippets.ignore_clippy
		result.encode_data += RustSnippets.encode_data_import
		result.encode_data += RustSnippets.encode_mock

		result.decode_master_mapping += RustSnippets.decode_master_mapping_import
		result.decode_master_mapping += RustSnippets.decode_message_info
		result.decode_master_mapping += RustSnippets.decode_master_mapping_signature

		result.encode_master_mapping += RustSnippets.encode_master_mapping_import
		result.encode_master_mapping += RustSnippets.encode_message_info
		result.encode_master_mapping += RustSnippets.encode_master_mapping_signature

		list_of_keys = []
		for msg in msgs:
			result.decode_data += self.synthesize_decode(msg) + "\n"
			result.decode_master_mapping += self.map_msg_to_decoder(msg)

			if (msg["key"]):
				list_of_keys.append(msg["key"])
				result.encode_data += self.synthesize_encode(msg) + "\n"
				result.encode_master_mapping += self.map_msg_to_encoder(msg)


		result.encode_master_mapping += RustSnippets.encode_master_mapping_closing
		result.encode_master_mapping += self.create_encode_key(list_of_keys)

		result.decode_master_mapping += RustSnippets.decode_master_mapping_closing

		result.format_data = RustSnippets.format_impl


		return result

	def map_msg_to_decoder(self, msg: Dict) -> str:
		"""
		Helper function that maps a given Dict (representing a CANMsg) to its decode function
		"""
        _id = msg["id"]
        desc = msg["desc"]
		return f"    {_id} => DecodeMessageInfo::new({self.function_name_decode(desc)}),\n"

	def map_msg_to_encoder(self, msg: Dict) -> str:
		"""
		Helper function that maps a given Dict (representing a EncodableCANMsg) to its encode function
		"""
        key = msg["key"]
        desc = msg["desc"]
		return f"    \"{key}\" => EncodeMessageInfo::new({self.function_name_encode(desc)}),\n"

	def synthesize_decode(self, msg: Dict) -> str:
		"""
		Helper function that synthesizes the decode function for a given Dict (representing a CANMsg)
		"""

		# Generate Function Signature
		signature: str = self.signature_decode(msg["desc"])

		length_check: str = self.add_length_check(point for field in msg["fields"] for point in field["points"])

		# Generate a line for each field in the message
		generated_lines: list[str] = []
		generated_lines += self.parse_msg_decode(msg)

		# Combine everything into one string
		total_list: list[str] = (
			[signature, length_check,RustSnippets.bitreader_create, RustSnippets.decode_return_value]
			+ generated_lines
			+ [RustSnippets.decode_close]
		)
		return "\n".join(total_list)

	def synthesize_encode(self, msg: Dict) -> str:
		"""
		Helper function that synthesizes the encode function for a given EncodableCANMsg
		"""

		# Generate Function Signature
		signature: str = self.signature_encode(msg["desc"])

		# Generate a line for each field in the message
		generated_lines: list[str] = []
		generated_lines += self.parse_msg_encode(msg)

		ret_encode_inst = self.encode_data_inst(msg)

		# Combine everything into one string
		total_list: list[str] = (
			[signature, RustSnippets.bitwriter_create]
			+ generated_lines
			+ [ret_encode_inst] + ["}"]
		)
		return "\n".join(total_list)

	def parse_msg_decode(self, msg: Dict) -> list[str]:
		result = []

		# For all the fields in the CAN message, generate the required code
		for index, field in enumerate(msg["fields"], start=0):
				if field["send"]:
					topic_suffix: str = None
					# if we have a topic append, then we create a standalone variable to read the bits before we
					# push all the points
					if field["topic_append"]:
						# point to append
						topic_suffix_pt = field["points"].pop(0)
						# init a new variable with index at end to ensure uniqueness in scope
						result.append(f"let topic_suffix_{index} = {self.decode_field_value(topic_suffix_pt)};")
						# variable name to be given to format!
						topic_suffix = f"topic_suffix_{index}"

					result.append(f"    {RustSnippets.network_encoding_start}")

					# get the decoding string for each point and pass it into the final formatter
					values: str = f"{','.join(self.decode_field_value(point) for point in field['points'])}]"
					result.append(self.finalize_line(field.name, field.unit, values, topic_appends_name=topic_suffix))

					result.append(f"        {RustSnippets.network_encoding_closing}")
				elif index < len(msg["fields"]) -1:
					# if field isnt sent, still decode it to get to the next bits, but only if it isnt the last point
					# if it is the last point, we can just exit out and save the resources of decoding it
					result.append(f"            {','.join(self.decode_field_value(point, skip=True) for point in field['points'])};")

		return result

	def parse_msg_encode(self, msg: Dict) -> list[str]:
		result = []
		# flatten list to get points
		pts_ls: List = [pt for field in msg["fields"] for pt in field["points"]]
		# For all the points in the CAN message, generate the required code
		for index, pt in enumerate(pts_ls, start=0):
				result.append(self.encode_field_value(pt, index))


		return result

	def add_length_check(self, fields: List[Dict]) -> str:
		"""
		Adds a length checker to exit out if the message is too small, will still parse if too big
		"""
        # Replaces summation of get_size_bits() for field in fields
        field_size: int = 0
        for field in fields:
            num_bytes = field["size"] / 8
            field_size += (num_bytes + 1) if (num_bytes % 8) else num_bytes
		return f"    if data.len() < {int(field_size)} {{ return vec![]; }}"

	def decode_field_value(self, field: Dict, skip=False) -> str:
		"""
		Parse can point to do conversions on it, and maybe wrap in formatter
		"""
		return f"{self.decode_format_data(field, self.parse_decoders(field, skip))}"

	def encode_field_value(self, field: Dict, index: int) -> str:
		"""
		Parse can point to do conversions on it, and maybe wrap in formatter
		"""
		return f"	{self.parse_encoders(field, index)};"

	def function_name_decode(self, desc: str) -> str:
		"""
		Helper function that generates the name of a decode function for a
		given Dict (i.e. CANMsg) based off the can message description
		"""
		return f"decode_{desc.replace(' ', '_').lower()}"

	def function_name_encode(self, desc: str) -> str:
		"""
		Helper function that generates the name of a decode function for a
		given Dict (i.e. CANMsg) based off the can message description
		"""
		return f"encode_{desc.replace(' ', '_').lower()}"

	def signature_decode(self, desc: str) -> str:
		"""
		Helper function that generates the signature of a decode function for a
		given Dict (i.e. CANMsg) based off the can message description
		"""
		return f"pub fn {self.function_name_decode(desc)}(data: &[u8]) -> {RustSnippets.decode_return_type} {{"

	def signature_encode(self, desc: str) -> str:
		"""
		Helper function that generates the signature of a decode function for a
		given CANMsg based off the can message description
		"""
		return f"pub fn {self.function_name_encode(desc)}(data: Vec<f32>) -> {RustSnippets.encode_return_type} {{"

	def finalize_line(self, topic: str, unit: str, val: str, topic_appends_name: Optional[str] = None ) -> str:
		"""
		Helper function that generates a line the data struct for a given CANPoint value
		"""
		# basically attach the name of the variable of the mqtt data to the topic should it exist,
		# otherwise format the plain string topic (should compile out)
		format_topic: str = f'&format!("{topic}'
		if topic_appends_name is not None:
			format_topic += "/{}"
		format_topic += '", '
		if topic_appends_name is not None:
			format_topic += f'{topic_appends_name},'
		format_topic += ")"
		return f'    		 {val}, \n    {format_topic}, "{unit}")'

	def parse_decoders(self, field: Dict, skip) -> str:
		"""
		Helper function that parses the decoders for a given Dict (i.e. CANUnit) by applying the
		decoders to the data and casting the result to the final type of the Dict (i.e. CANUnit).
		"""

		if skip:
			return f"reader.skip({field['size']}).unwrap()"

		if field["endianness"] == "big":
			if field["signed"]:
				# doesnt need exact sign bit as it is in big endian form, the form of the native stream
				base = f"reader.read_signed_in::<{field['size']}, i32>().unwrap()"
			else:
				base = f"reader.read_in::<{field['size']}, u32>().unwrap()"
		elif field["endianness"] == "little":
            # math.ceil(...) replaces get_size_min_bytes()
            if (field["size"] != (math.ceil(field["size"] / 8.0) * 8)):
				print("You cannot have a non byte sized signed number!")
				exit(1)
			# use the read_as_to, which requires byte sized data to get the correct sign bit
			if field["signed"]:
				base = f"reader.read_as_to::<LittleEndian, i{field['size']}>().unwrap()"
			else:
				base = f"reader.read_as_to::<LittleEndian, u{field['size']}>().unwrap()"
		else:
			print("Invalid endianness: ", field["endianness"])
			exit(1)

		return f"{base} as {field['final_type']}"

	def parse_encoders(self, field: Dict, index: int) -> str:
		"""
		Helper function that parses the encoders for a given Dict (i.e. CANUnit) by applying the
		encoders to the data and casting the result to the final type of the Dict (i.e. CANUnit).
		"""

		default_statement = f"&({field['default']}_f32)"
		float_final = self.encode_format_data(field, f"*data.get({index}).unwrap_or({default_statement})")
        field_size_min_bytes = math.ceil(field["size"] / 8.0) * 8

		if field["endianness"] == "big":
			if field["signed"]:
				# doesnt need exact sign bit as it is in big endian form, the form of the native stream
				base = f"writer.write_signed_out::<{field['size']}, i{field_size_min_bytes}>({float_final} as i{field_size_min_bytes}).unwrap()"
			else:
				base = f"writer.write_out::<{field['size']}, u{field_size_min_bytes}>({float_final} as u{field_size_min_bytes}).unwrap()"
		elif field["endianness"] == "little":
			if (field["size"] != field_size_min_bytes):
				print("You cannot have a non byte sized signed number!")
				exit(1)

			if field["signed"]:
				base = f"writer.write_as_from::<LittleEndian, i{field['size']}>({float_final} as i{field['size']}).unwrap()"
			else:
				base = f"writer.write_as_from::<LittleEndian, u{field['size']}>({float_final} as u{field['size']}).unwrap()"
		else:
			print("Invalid endianness: ", field["endianness"])
			exit(1)

		return base

	def decode_format_data(self, field: Dict, decoded_data: str) -> str:
		"""
		Helper function that formats the data for a given Dict (i.e. CANPoint) based off the
		format of the Dict (i.e. CANPoint) if it exists, for the decoding
		"""
		cf = decoded_data
		if field["format"]:
			cf = f"FormatData::{field['format']}_d({decoded_data})"
		return cf

	def encode_format_data(self, field: Dict, encoded_data: str) -> str:
		"""
		Helper function that formats the data for a given Dict (i.e. CANPoint) based off the
		format of the Dict (i.e. CANPoint) if it exists, for the encoding alternative
		"""
		cf = encoded_data
		if field["format"]:
			cf = f"FormatData::{field['format']}_e({encoded_data})"
		return cf

	def create_encode_key(self, list_of_keys) -> str:
		lr = f"pub const ENCODABLE_KEY_LIST: [&str; {len(list_of_keys)}] = [\n"
		for k in list_of_keys:
			lr += f"\"{k}\",\n"
		lr += "];\n"

		return lr

	def encode_data_inst(self, msg: Dict) -> str:
		return f"""
	EncodeData {{
		value: writer.into_writer(),
		id: {int(msg['id'], 16)},   // {msg['id']}
		is_ext: {str(msg['is_ext']).lower()},
	}}"""



class RustSnippets:
	"""
	Container for all the Rust code needed for running and compilation
	"""

	ignore_clippy: str = (
		"#![allow(clippy::all)]\n"  # Ignoring clippy for decode_data because it's autogenerated and has some unnecessary type casting to ensure correct types
	)

	format_impl = """
/**
 * Class to contain the data formatting functions
 * _d = a func to decode a value
 * _e = its counterpart to encode a value for sending on CAN line
 */
pub struct FormatData {}

impl FormatData {
	pub fn divide10_d(value: f32) -> f32 {
		value / 10.0
	}
	pub fn divide10_e(value: f32) -> f32 {
		value * 10.0
	}

	pub fn divide100_d(value: f32) -> f32 {
		value / 100.0
	}
	pub fn divide100_e(value: f32) -> f32 {
		value * 100.0
	}

	pub fn divide10000_d(value: f32) -> f32 {
		value / 10000.0
	}
	pub fn divide10000_e(value: f32) -> f32 {
		value * 10000.0
	}

	/* Acceleration values must be offset by 0.0029 according to datasheet */
	pub fn acceleration_d(value: f32) -> f32 {
		value * 0.0029
	}
	pub fn acceleration_e(value: f32) -> f32 {
		value / 0.0029
	}
}"""

	bitreader_create = "    let mut reader = BitReader::endian(Cursor::new(&data), BigEndian);"
	bitwriter_create = "    let mut writer = BitWriter::endian(Vec::new(), BigEndian);"

	decode_data_import: str = (
		"""
		use crate::format_data::FormatData;
		use super::data::DecodeData;
			use std::io::Cursor;
			use bitstream_io::{BigEndian, LittleEndian, BitReader, BitRead};\n
		  """  # Importing the Data struct and the bistream io libraries
	)
	encode_data_import: str = (
		"""
		use crate::format_data::FormatData;
		use super::data::EncodeData;
			use bitstream_io::{BigEndian, LittleEndian, BitWriter, BitWrite};\n
		  """  # Importing the Data struct and the bistream io libraries
	)

	decode_return_type: str = "Vec::<DecodeData>"  # The return type of any decode function
	encode_return_type: str = "EncodeData"  # The return type of any decode function
	decode_return_value: str = (
		f"    let mut result: Vec<DecodeData> = Vec::new();"  # Initializing the result vector
	)
	decode_close: str = (
		"	result\n}\n"  # Returning the result vector and closing the function
	)

	decode_mock: str = """
pub fn decode_mock(_data: &[u8]) -> Vec::<DecodeData> {
	let result = vec![
	DecodeData::new(vec![0.0], "Calypso/Unknown", "")
	];
	result
}\n"""  # A debug decode function that is used for messages that don't have a decode function

	encode_mock: str = """
pub fn encode_mock(data: Vec<f32>) -> EncodeData {
    let mut writer = BitWriter::endian(Vec::new(), BigEndian);
    writer.write_from::<u8>(data.len() as u8).unwrap();
    EncodeData {
        value: writer.into_writer(),
        id: 2047, // 0x7FF
        is_ext: false,
    }
}\n"""  # A debug decode function that is used for messages that don't have a decode function

	network_encoding_start: str = "result.push(DecodeData::new(vec!["
	network_encoding_closing: str = ");"

	decode_master_mapping_import: str = (
		"use super::decode_data::*;\nuse super::data::DecodeData;\n"  # Importing all the functions in decode_data.rs file and the Data struct
	)

	encode_master_mapping_import: str = (
		"use super::encode_data::*;\nuse super::data::EncodeData;\n"  # Importing all the functions in encode_data.rs file and the Data struct
	)

	decode_master_mapping_signature: str = (
		"pub fn get_message_info(id: &u32) -> DecodeMessageInfo {\n   match id {"  # The signature of the master_mapping function
	)

	encode_master_mapping_signature: str = (
		"pub fn get_message_info(key: String) -> EncodeMessageInfo {\n   let key_owned = key.as_str();\n	match key_owned {"
		# The signature of the master_mapping function
	)

	decode_master_mapping_closing: str = (
		"    _ => DecodeMessageInfo::new(decode_mock),\n    }\n}\n"  # The closing of the master_mapping function and the default case for the match statement that returns the mock decode function
	)

	encode_master_mapping_closing: str = (
		"    _ => EncodeMessageInfo::new(encode_mock),\n    }\n}\n"  # The closing of the master_mapping function and the default case for the match statement that returns the mock encode function
	)


	decode_message_info = """
	pub struct DecodeMessageInfo {
		pub decoder: fn(data: &[u8]) -> Vec<DecodeData>,
	}

	impl DecodeMessageInfo {
		pub fn new(decoder: fn(data: &[u8]) -> Vec<DecodeData>) -> Self {
			Self {
				decoder
			}
		}
	}
	""" # The MessageInfo struct that is used to store the decode function for a given message

	encode_message_info = """
	pub struct EncodeMessageInfo {
    	pub encoder: fn(data: Vec<f32>) -> EncodeData,
	}

	impl EncodeMessageInfo {
    	pub fn new(encoder: fn(data: Vec<f32>) -> EncodeData) -> Self {
        	Self { encoder }
    	}
	}
	""" # The MessageInfo struct that is used to store the decode function for a given message

