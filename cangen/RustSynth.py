from cangen.CANField import CANField
from cangen.CANMsg import CANMsg
from cangen.Result import Result
from typing import List

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
		result.decode_data += RustSnippets.bitreader_impl
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

		length_check: str = self.add_length_check(msg.fields)

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
		for field in msg.fields:

			# If the field is discrete
			if field.field_type == "discrete":
				result.append(f"        {RustSnippets.network_encoding_start}")
				result.append(f"             {self.decode_field_value(field)}")
				result.append(f"        {RustSnippets.network_encoding_closing}")
				result.append(f'        , "{field.name}", "{field.unit}"),')

			# If the data is composite
			elif field.field_type == "composite":
				result.append(f"        {RustSnippets.network_encoding_start}")
				result.append(
					f"            {','.join(self.decode_field_value(field) for _ in range(field.num_points))}"
				)
				result.append(f"        {RustSnippets.network_encoding_closing}")
				result.append(
					f'        , "{field.name}", "{field.unit}")'
				)
			else:
				print("field type not recognized!\n")
				exit(1)

		return result

	def add_length_check(self, fields: List[CANField]) -> str:
		field_size = sum(field.get_size_bytes() for field in fields)
		return f"    if data.len() < {int(field_size)} {{ return vec![]; }}"

	def decode_field_value(self, field: CANField) -> str:
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

	def finalize_line(self, topic: str, unit: str, val: str) -> str:
		"""
		Helper function that generates a line the data struct for a given CANField value
		"""
		return f'    Data::new({val}, "{topic}", "{unit}"),'

	def parse_decoders(self, field: CANField) -> str:
		"""
		Helper function that parses the decoders for a given CANField by applying the
		decoders to the data and casting the result to the final type of the CANField.
		"""
		base = f"reader.read_bits({field.size})"

		if field.endianness == "big":
			base = f"{base}.to_be()"

		# TODO: Make this configurable based on endianness of platform
		#elif field.endianness == "little":
		#    base = f"{base}.to_le()"

		if field.signed:
			base = f"({base} as i{field.get_size_bytes() * 8}).wrapping_neg()"

		return f"{base} as {field.final_type}"

	def format_data(self, field: CANField, decoded_data: str) -> str:
		"""
		Helper function that formats the data for a given CANField based off the
		format of the CANField if it exists, returns the decoded data otherwise
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

	bitreader_impl: str = ("""
/* Struct to decode a byte array bit by bit, sequentially */
struct BitReader<'a> {
	data: &'a [u8],
	byte_pos: usize,
	bit_pos: u8,
}

impl<'a> BitReader<'a> {
	fn new(data: &'a [u8]) -> Self {
		Self {
			data,
			byte_pos: 0,
			bit_pos: 0,
		}
	}

	fn read_bits(&mut self, num_bits: u8) -> u32 {
		let mut value: u32 = 0;
		for _ in 0..num_bits {
			let bit = (self.data[self.byte_pos] >> (7 - self.bit_pos)) & 1;
			value = (value << 1) | bit as u32;

			self.bit_pos += 1;
			if self.bit_pos == 8 {
				self.bit_pos = 0;
				self.byte_pos += 1;
			}
		}
		value
	}
}
"""
	)

	format_impl = """
/**
 * Class to contain the data formatting functions
 */
pub struct FormatData {}

impl FormatData {
	/* Temperatures are divided by 10 for 1 decimal point precision in C */
	pub fn temperature(value: f32) -> f32 {
		value / 10.0
	}

	/* Torque values are divided by 10 for one decimal point precision in N-m */
	pub fn torque(value: f32) -> f32 {
		value / 10.0
	}

	/* Current values are divided by 10 for one decimal point precision in A */
	pub fn current(value: f32) -> f32 {
		value / 10.0
	}

	/* Cell Voltages are recorded on a 10000x multiplier for V, must be divided by 10000 to get accurate number */
	pub fn cell_voltage(value: f32) -> f32 {
		value / 10000.0
	}

	/* Acceleration values must be offset by 0.0029 according to datasheet */
	pub fn acceleration(value: f32) -> f32 {
		value * 0.0029
	}

	/* High Voltage values are divided by 100 for one decimal point precision in V, high voltage is in regards to average voltage from the accumulator pack */
	pub fn high_voltage(value: f32) -> f32 {
		value / 100.0
	}
}"""

	bitreader_create = "    let mut reader = BitReader::new(data);"

	decode_data_import: str = (
		"use super::data::Data;\n"  # Importing the Data struct and the FormatData and ProcessData traits
	)

	decode_return_type: str = "Vec::<Data>"  # The return type of any decode function
	decode_return_value: str = (
		f"    let result = vec!["  # Initializing the result vector
	)
	decode_close: str = (
		"    ];\n    result\n}\n"  # Returning the result vector and closing the function
	)

	decode_mock: str = """
pub fn decode_mock(_data: &[u8]) -> Vec::<Data> {
	let result = vec![
	Data::new(vec![0.0], "Mock", "")
	];
	result
}"""  # A mock decode function that is used for messages that don't have a decode function

	network_encoding_start: str = "Data::new(vec!["
	network_encoding_closing: str = "]"

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

