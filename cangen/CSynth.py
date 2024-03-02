from cangen.CANField import CANField
from cangen.CANMsg import CANMsg
from cangen.Result import Result
from typing import List

"""
Example prototype

void compute_send_mc_message(uint16_t user_max_charge, uint16_t user_max_discharge)
{
	struct __attribute__((packed)) {
		uint16_t maxDischarge;
		uint16_t maxCharge;
	} mc_msg_data;

	mc_msg_data.maxCharge	= user_max_charge;
	mc_msg_data.maxDischarge = user_max_discharge;

	can_msg_t mc_msg;
	mc_msg.id = 0x78;
	mc_msg.len = sizeof(mc_msg_data);
	memcpy(mc_msg.data, &mc_msg_data, sizeof(mc_msg_data));

	can_send_msg(&can1, &mc_msg);
}
"""

class CSynth:
	"""
	A class to synthesize C from a given CANMsg spec.
	"""

	decode_data_import: str = (
		'#include "can.h" \n#include <stdio.h>\n\n'  # Importing the Data struct and the FormatData and ProcessData traits
	)

	decode_return_type: str = "void"  # The return type of any decode function
	packed_struct_value: str = (
		f"    struct __attribute__((packed)) {{"  # Initializing the result vector
	)

	packed_struct_end: str = "    } msg_data;\n"

	decode_close: str = (
		"    ]; \n    result\n}\n"  # Returning the result vector and closing the function
	)

	master_mapping_import: str = (
		"use super::decode_data::*; \nuse super::data::Data; \n"  # Importing all the functions in decode_data.rs file and the Data struct
	)

	master_mapping_signature: str = (
		"pub fn get_message_info(id: &u32) -> MessageInfo { \n   match id {"  # The signature of the master_mapping function
	)

	master_mapping_closing: str = (
		"    _ => MessageInfo::new(decode_mock), \n    }\n}"  # The closing of the master_mapping function and the default case for the match statement that returns the mock decode function
	)

	# The main function of the CSynth class. Takes a list of CANMsgs and returns a Result object that contains the synthesized C code
	def parse_messages(self, msgs: List[CANMsg]) -> Result:
		result = Result("", "")

		result.decode_data += self.decode_data_import
		#result.decode_data += self.decode_mock

		#result.master_mapping += self.master_mapping_import
		#result.master_mapping += self.message_info
		#result.master_mapping += self.master_mapping_signature

		for msg in msgs:
			result.decode_data += self.synthesize(msg) + "\n"
			#result.master_mapping += self.map_msg_to_decoder(msg)

		#result.master_mapping += self.master_mapping_closing
		return result

	# Helper function that maps a given CANMsg to its decode function
	def map_msg_to_decoder(self, msg: CANMsg) -> str:
		return f"    {msg.id} => MessageInfo::new({self.function_name(msg.desc)}),\n"

	# Helper function that synthesizes the decode function for a given CANMsg
	def synthesize(self, msg: CANMsg) -> str:
		signature: str = self.signature(msg.desc)
		length_check: str = self.add_length_check(msg.networkEncoding[0].fields)
		generated_lines: list[str] = []
		print(self.packed_struct_end)
		# Generate a line for each field in the message
		generated_lines += self.parse_network_encoding(msg)
		total_list: list[str] = (
			[signature, self.packed_struct_value]
			+ generated_lines
			+ [self.packed_struct_end]
			+ ["}\n"]
		)
		return "\n".join(total_list)

	def parse_network_encoding(
		self, msg: CANMsg
	) -> list[str]:  # Change return type to list[str]
		result = []
		networkEncoding = msg.networkEncoding[0]
		if networkEncoding.id == "csv":
			result.append(
				f"            {','.join(self.decode_field_value(field) for field in networkEncoding.fields)}"
			)
			result.append(
				f'        , "{networkEncoding.topic}", "{networkEncoding.unit}")'
			)
		elif networkEncoding.id == "single_point":
			for field in networkEncoding.fields:
				result.append(f"             {self.decode_field_value(field)}")
				result.append(f'        , "{field.name}", "{field.unit}"), ')
		return result

	def add_length_check(self, fields: List[CANField]) -> str:
		fieldSize = sum(field.size for field in fields)
		return f"if data.len() < {fieldSize} {{ return vec![]; }}"

	def decode_field_value(self, field: CANField) -> str:
		return f"{self.format_data(field, self.parse_decoders(field))}"

	# Helper function that generates the name of a decode function for a given CANMsg based off the can message description
	def function_name(self, desc: str) -> str:
		return f"send_{desc.replace(' ', '_').lower()}"

	# Helper function that generates the signature of a decode function for a given CANMsg based off the can message description
	def signature(self, desc: str) -> str:
		return f"{self.decode_return_type} {self.function_name(desc)}(uint8_t data[], uint8_t len)\n{{"

	# Helper function that generates a line the data struct for a given CANField value
	def finalize_line(self, topic: str, unit: str, val: str) -> str:
		return f'    Data::new({val}, "{topic}", "{unit}"),'

	# Helper function that parses the decoders for a given CANField by applying the decoders to the data and casting the result to the final type of the CANField.
	def parse_decoders(self, field: CANField) -> str:
		if isinstance(field.decodings, type(None)):
			return f"data[{field.index}] as {field.final_type}"
		else:
			base: str
			if field.size == 1:
				base = f"data[{field.index}]"
			else:
				base = f"&data[{field.index}..{field.index + field.size}]"
			for decoder in field.decodings:
				base = f"pd::{decoder.repr}({base} as {decoder.entry_type}, {decoder.bits})"
			return f"{base} as {field.final_type}"

	# Helper function that formats the data for a given CANField based off the format of the CANField if it exists, returns the decoded data otherwise
	def format_data(self, field: CANField, decoded_data: str) -> str:
		cf = decoded_data
		if field.format:
			cf = f"fd::{field.format}({decoded_data})"
		return cf
