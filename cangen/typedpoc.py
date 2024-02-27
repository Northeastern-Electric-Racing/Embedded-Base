from YAMLParser import YAMLParser
from RustSynth import RustSynth

decode_data = open("../src/decode_data.rs", "w")
master_mapping = open("../src/master_mapping.rs", "w")

bms_messages = YAMLParser().parse(open("can-messages/bms.yaml", "r"))
mpu_messages = YAMLParser().parse(open("can-messages/mpu.yaml", "r"))
wheel_messages = YAMLParser().parse(open("can-messages/wheel.yaml", "r"))


bms_messages.msgs.extend(mpu_messages.msgs)
bms_messages.msgs.extend(wheel_messages.msgs)

result = RustSynth().parse_messages(bms_messages.msgs)

decode_data.write(result.decode_data)
decode_data.close()

master_mapping.write(result.master_mapping)
master_mapping.close()