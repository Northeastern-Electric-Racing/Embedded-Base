from cangen.CANField import CANField
from cangen.CANMsg import CANMsg
from cangen.Result import Result
from typing import List

"""
Example prototype

void compute_send_mc_message(can_t *can, uint16_t user_max_charge, uint16_t user_max_discharge)
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

	# The main function of the CSynth class. Takes a list of CANMsgs and returns a Result object that contains the synthesized C code
	def parse_messages(self, msgs: List[CANMsg]) -> Result:
		result = Result("", "")

		result.decode_data += self.decode_data_import

		for msg in msgs:
			result.decode_data += self.synthesize(msg) + "\n"

		return result

	# Helper function that synthesizes the decode function for a given CANMsg
	def synthesize(self, msg: CANMsg) -> str:
		signature: str = self.signature(msg.desc, msg.networkEncoding[0].fields)
		generated_lines: list[str] = []

		# Generate a line for each field in the message
		generated_lines += self.parse_network_encoding(msg)
		total_list: list[str] = (
			[signature, self.packed_struct_value]
			+ generated_lines
			+ [self.packed_struct_end]
			+ [self.format_msg(msg)]
			+ [self.decode_close]
		)
		return "\n".join(total_list)

	def format_msg (self, msg: CANMsg) -> str:
		return f"    can_msg_t mc_msg;\n    mc_msg.id = {msg.id};\n    mc_msg.len = sizeof(mc_msg_data);"

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
				result.append(f"        {self.decode_field_value(field)} {field.name.replace('/', '_').lower()};\t/* {field.unit} */")
		return result

	def decode_field_value(self, field: CANField) -> str:
		return f"{self.format_data(field, self.parse_decoders(field))}"

	# Helper function that generates the name of a decode function for a given CANMsg based off the can message description
	def function_name(self, desc: str) -> str:
		return f"send_{desc.replace(' ', '_').lower()}"

	# Helper function that generates the signature of a decode function for a given CANMsg based off the can message description
	def signature(self, desc: str, fields: list[CANField]) -> str:
		params : str = ""

		for field in fields:
			print(f'{self.format_data(field, self.parse_decoders(field))}, ')
			params.concat(f'{self.format_data(field, self.parse_decoders(field))}, ')

		return f"{self.decode_return_type} {self.function_name(desc)}(can_t *can, {params}uint8_t len)\n{{"

	# Helper function that generates a line the data struct for a given CANField value
	def finalize_line(self, topic: str, unit: str, val: str) -> str:
		return f'    Data::new({val}, "{topic}", "{unit}"),'

	# Helper function that parses the decoders for a given CANField by applying the decoders to the data and casting the result to the final type of the CANField.
	def parse_decoders(self, field: CANField) -> str:
		if isinstance(field.encodings, type(None)):
			return f"uint8_t "
		else:
			base: str
			if field.size == 1:
				base = f"data[{field.index}]"
			else:
				base = f"&data[{field.index}..{field.index + field.size}]"
			for decoder in field.encodings:
				base = f"{decoder.entry_type}"
			return f"{base}"

	# Helper function that formats the data for a given CANField based off the format of the CANField if it exists, returns the decoded data otherwise
	def format_data(self, field: CANField, decoded_data: str) -> str:
		cf = decoded_data
		return cf

class CSnippets:
	"""
	Container for the code needed for C compilation and running
	"""

	decode_data_import: str = (
		'#include <stdio.h>\n#include <stdint.h>\n#include "can.h"\n\n'  # Including the necessary headers
	)

	decode_return_type: str = "void"  # The return type of any decode function
	packed_struct_value: str = (
		f"    struct __attribute__((packed)) {{" # Initializing the packed strucct
	)

	packed_struct_end: str = "    } msg_data;\n"

	decode_close: str = ("    can_send_msg(can, &mc_msg);\n}\n"  # Sending the CAN message
	)
