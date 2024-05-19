from cangen.CANField import CANPoint
from cangen.CANMsg import CANMsg
from cangen.Result import Result
from typing import List, Optional

class RustSynth:
	"""
	A class to synthesize Rust from a given CANMsg spec.
	"""

	def parse_messages(self, msgs: List[CANMsg]) -> Result:
		"""
		The main function of the RustSynth class. Takes a list of CANMsgs and
		returns a Result object that contains the synthesized Rust code for the
		decode_data.rs and master_mapping.rs files
		"""
		result = Result("", "")
		result.decode_data += RustSnippets.ignore_clippy
		# result.decode_data += RustSnippets.bitreader_impl
		result.decode_data += RustSnippets.format_impl
		result.decode_data += RustSnippets.decode_data_import
		result.decode_data += RustSnippets.decode_mock

		result.master_mapping += RustSnippets.master_mapping_import
		result.master_mapping += RustSnippets.message_info
		result.master_mapping += RustSnippets.master_mapping_signature

		for msg in msgs:
			result.decode_data += self.synthesize(msg) + "\n"
			result.master_mapping += self.map_msg_to_decoder(msg)

		result.master_mapping += RustSnippets.master_mapping_closing
		return result

	def map_msg_to_decoder(self, msg: CANMsg) -> str:
		"""
		Helper function that maps a given CANMsg to its decode function
		"""
		return f"    {msg.id} => MessageInfo::new({self.function_name(msg.desc)}),\n"

	def synthesize(self, msg: CANMsg) -> str:
		"""
		Helper function that synthesizes the decode function for a given CANMsg
		"""

		# Generate Function Signature
		signature: str = self.signature(msg.desc)

		length_check: str = self.add_length_check(point for field in msg.fields for point in field.points)

		# Generate a line for each field in the message
		generated_lines: list[str] = []
		generated_lines += self.parse_msg(msg)

		# Combine everything into one string
		total_list: list[str] = (
			[signature, length_check,RustSnippets.bitreader_create, RustSnippets.decode_return_value]
			+ generated_lines
			+ [RustSnippets.decode_close]
		)
		return "\n".join(total_list)

	def parse_msg(self, msg: CANMsg) -> list[str]:
		result = []

		# For all the fields in the CAN message, generate the required code
		for index, field in enumerate(msg.fields, start=0):
				if field.send:
					topic_suffix: str = None
					# if we have a topic append, then we create a standalone variable to read the bits before we 
					# push all the points
					if field.topic_append:
						# point to append
						topic_suffix_pt = field.points.pop(0)
						# init a new variable with index at end to ensure uniqueness in scope
						result.append(f"let topic_suffix_{index} = {self.decode_field_value(topic_suffix_pt)};")
						# variable name to be given to format!
						topic_suffix = f"topic_suffix_{index}"

					result.append(f"    {RustSnippets.network_encoding_start}")

					# get the decoding string for each point and pass it into the final formatter
					values: str = f"{','.join(self.decode_field_value(point) for point in field.points)}]"
					result.append(self.finalize_line(field.name, field.unit, values, topic_appends_name=topic_suffix))

					result.append(f"        {RustSnippets.network_encoding_closing}")
				elif index < len(msg.fields) -1: 
					# if field isnt sent, still decode it to get to the next bits, but only if it isnt the last point
					# if it is the last point, we can just exit out and save the resources of decoding it
					result.append(f"            {','.join(self.decode_field_value(point, skip=True) for point in field.points)};")


		return result

	def add_length_check(self, fields: List[CANPoint]) -> str:
		"""
		Adds a length checker to exit out if the message is too small, will still parse if too big
		"""
		field_size = sum(field.get_size_bits() for field in fields) / 8
		return f"    if data.len() < {int(field_size)} {{ return vec![]; }}"

	def decode_field_value(self, field: CANPoint) -> str:
		"""
		Parse can point to do conversions on it, and maybe wrap in formatter
		"""
		return f"{self.format_data(field, self.parse_decoders(field))}"

	def function_name(self, desc: str) -> str:
		"""
		Helper function that generates the name of a decode function for a
		given CANMsg based off the can message description
		"""
		return f"decode_{desc.replace(' ', '_').lower()}"

	def signature(self, desc: str) -> str:
		"""
		Helper function that generates the signature of a decode function for a
		given CANMsg based off the can message description
		"""
		return f"pub fn {self.function_name(desc)}(data: &[u8]) -> {RustSnippets.decode_return_type} {{"

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

	def parse_decoders(self, field: CANPoint, skip=False) -> str:
		"""
		Helper function that parses the decoders for a given CANUnit by applying the
		decoders to the data and casting the result to the final type of the CANUnit.
		"""

		if skip:
			return f"reader.skip::({field.size}).unwrap()"
		
		size = field.size
		if field.size < 8:
			size = 8

		if field.signed:
			base = f"reader.read_signed::<i{size}>({field.size}).unwrap()"
		else:
			base = f"reader.read::<u{size}>({field.size}).unwrap()"

			

		return f"{base} as {field.final_type}"

	def format_data(self, field: CANPoint, decoded_data: str) -> str:
		"""
		Helper function that formats the data for a given CANPoint based off the
		format of the CANPoint if it exists
		"""
		cf = decoded_data
		if field.format:
			cf = f"FormatData::{field.format}({decoded_data})"
		return cf


class RustSnippets:
	"""
	Container for all the Rust code needed for running and compilation
	"""

	ignore_clippy: str = (
		"#![allow(clippy::all)]\n"  # Ignoring clippy for decode_data because it's autogenerated and has some unnecessary type casting to ensure correct types
	)

# 	bitreader_impl: str = ("""
# /* Struct to decode a byte array bit by bit, sequentially */
# struct BitReader<'a> {
# 	data: &'a [u8],
# 	byte_pos: usize,
# 	bit_pos: u8,
# }

# impl<'a> BitReader<'a> {
# 	fn new(data: &'a [u8]) -> Self {
# 		Self {
# 			data,
# 			byte_pos: 0,
# 			bit_pos: 0,
# 		}
# 	}

# 	fn read_bits(&mut self, num_bits: u8) -> u32 {
# 		let mut value: u32 = 0;
# 		for _ in 0..num_bits {
# 			let bit = (self.data[self.byte_pos] >> (7 - self.bit_pos)) & 1;
# 			value = (value << 1) | bit as u32;

# 			self.bit_pos += 1;
# 			if self.bit_pos == 8 {
# 				self.bit_pos = 0;
# 				self.byte_pos += 1;
# 			}
# 		}
# 		value
# 	}
# }
# """
#	)

	format_impl = """
/**
 * Class to contain the data formatting functions
 */
pub struct FormatData {}

impl FormatData {
	pub fn divide10(value: f32) -> f32 {
		value / 10.0
	}

	pub fn divide100(value: f32) -> f32 {
		value / 100.0
	}

	pub fn divide10000(value: f32) -> f32 {
		value / 10000.0
	}

	/* Acceleration values must be offset by 0.0029 according to datasheet */
	pub fn acceleration(value: f32) -> f32 {
		value * 0.0029
	}
}"""

	bitreader_create = "    let mut reader = BitReader::endian(Cursor::new(&data), BigEndian);"

	decode_data_import: str = (
		"""use super::data::Data;
			use std::io::{Read, Cursor};
			use bitstream_io::{BigEndian, BitReader, BitRead};\n
		  """  # Importing the Data struct and the FormatData and ProcessData traits
	)

	decode_return_type: str = "Vec::<Data>"  # The return type of any decode function
	decode_return_value: str = (
		f"    let mut result: Vec<Data> = Vec::new();"  # Initializing the result vector
	)
	decode_close: str = (
		"	result\n}\n"  # Returning the result vector and closing the function
	)

	decode_mock: str = """
pub fn decode_mock(_data: &[u8]) -> Vec::<Data> {
	let result = vec![
	Data::new(vec![0.0], "Calypso/Unknown", "")
	];
	result
}\n"""  # A debug decode function that is used for messages that don't have a decode function

	network_encoding_start: str = "result.push(Data::new(vec!["
	network_encoding_closing: str = ");"

	master_mapping_import: str = (
		"use super::decode_data::*;\nuse super::data::Data;\n"  # Importing all the functions in decode_data.rs file and the Data struct
	)

	master_mapping_signature: str = (
		"pub fn get_message_info(id: &u32) -> MessageInfo {\n   match id {"  # The signature of the master_mapping function
	)

	master_mapping_closing: str = (
		"    _ => MessageInfo::new(decode_mock),\n    }\n}"  # The closing of the master_mapping function and the default case for the match statement that returns the mock decode function
	)

	message_info = """
	pub struct MessageInfo {
		pub decoder: fn(data: &[u8]) -> Vec<Data>,
	}

	impl MessageInfo {
		pub fn new(decoder: fn(data: &[u8]) -> Vec<Data>) -> Self {
			Self {
				decoder
			}
		}
	}
	""" # The MessageInfo struct that is used to store the decode function for a given message

