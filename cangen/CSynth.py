from cangen.CANField import CANField
from cangen.CANMsg import CANMsg
from cangen.Result import Result
from typing import List

class CSynth:
	"""
	A class to synthesize C from a given CANMsg spec.
	"""

	def parse_messages(self, msgs: List[CANMsg]) -> Result:
		"""
		The main function of the CSynth class. Takes a list of CANMsgs and returns
		a Result object that contains the synthesized C code
		"""
		result = Result("", "")

		result.decode_data += CSnippets.decode_data_import

		result.master_mapping += CSnippets.master_mapping_signature

		for msg in msgs:
			result.master_mapping += self.map_msg_to_decoder(msg)
			result.decode_data += self.synthesize(msg) + "\n"

		result.master_mapping += CSnippets.master_mapping_closing

		return result

	def synthesize(self, msg: CANMsg) -> str:
		"""
		Helper function that synthesizes the decode function for a given CANMsg
		"""
		signature: str = self.signature(msg.desc, msg.fields)
		generated_lines: list[str] = []

		# Generate a line for each field in the message
		generated_lines += self.parse_msg(msg)
		total_list: list[str] = (
			[signature, CSnippets.packed_struct_value]
			+ generated_lines
			+ [CSnippets.packed_struct_end]
			+ [self.set_data(msg)]
			+ [self.format_msg(msg)]
			+ [CSnippets.decode_close]
		)
		return "\n".join(total_list)

	def map_msg_to_decoder(self, msg: CANMsg) -> str:
		"""
		Helper function that maps a given CANMsg to its decode function
		"""
		return f"        case {msg.id}:\n \n            break;\n"

	def parse_msg(self, msg: CANMsg) -> list[str]:
		result = []

		for field in msg.fields:

			# If the field is discrete
			if field.field_type == "discrete":
				result.append(f"        {self.create_variable(field)} : {field.size};\t/* {field.unit} */")

			# If the data is composite
			elif field.field_type == "composite":
				result.append(
					f"            {','.join(self.create_variable(field) for field in msg.fields)}"
				)
				result.append(
					f'        , "{msg.topic}", "{msg.unit}")'
				)
			else:
				print("field type not recognized!\n")
				exit(1)

		return result

	def set_data (self, msg: CANMsg) -> str:
		result = ""

		for field in msg.fields:
			result += f"    msg_data.{'_'.join(field.name.split('/')[-2:]).lower()} = {'_'.join(field.name.split('/')[-2:]).lower()};\n"

		return result

	def format_msg (self, msg: CANMsg) -> str:
		"""
		Helper function that generates the actual sending of the message
		"""
		return f"""
	can_msg_t msg;
	msg.id = {msg.id};
	msg.len = sizeof(msg_data);
	memcpy(msg.data, &msg_data, sizeof(msg_data));"""

	def function_name(self, desc: str) -> str:
		"""
		Helper function that generates the name of a decode function for a given CANMsg based off the can message description
		"""
		return f"send_{desc.replace(' ', '_').lower()}"

	def signature(self, desc: str, fields: list[CANField]) -> str:
		"""
		Helper function that generates the signature of a decode function for a given CANMsg based off the can message description
		"""
		params : str = ""

		for field in fields:
			params += f'{self.create_variable(field)}, '

		return f"{CSnippets.decode_return_type} {self.function_name(desc)}(can_t *can, {params}uint8_t len) {{"

	def finalize_line(self, topic: str, unit: str, val: str) -> str:
		"""
		Helper function that generates a line the data struct for a given CANField value
		"""
		return f'    Data::new({val}, "{topic}", "{unit}"),'

	def create_variable(self, field: CANField) -> str:
		"""
		Helper function that parses the decoders for a given CANField by
		applying the decoders to the data and casting the result to the final type of the CANField.
		"""
		if field.signed:
			return f"int{field.get_size_bytes() * 8 }_t {self.reference_variable(field)}"
		else:
			return f"uint{field.get_size_bytes() * 8 }_t {self.reference_variable(field)}"

	def reference_variable(self, field: CANField) -> str:
		"""
		Helper function to get a reference to the variable name without the data type
		"""
		return f"{'_'.join(field.name.split('/')[-2:]).lower()}"

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

	master_mapping_signature: str = (
		"    switch (message.id) {\n"
	)

	master_mapping_closing: str = (
		"        default:\n            break;\n        };\n    }"  # The closing of the master_mapping function and the default case for the match statement that returns the mock decode function
	)
