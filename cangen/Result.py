
class Result:
   """
   This class is used to store the results of the RustSynth.py script.
   decode_data is the synthesized Rust code for the decode_data.rs file.
   master_mapping is the synthesized Rust code for the master_mapping.rs file.
   """
   decode_data: str
   decode_master_mapping: str
   encode_data: str
   encode_master_mapping: str
   format_data: str

   def __init__(self, decode_data: str, master_mapping: str, encode_data: str, encode_master_mapping: str, format_data: str):
      self.decode_data = decode_data
      self.decode_master_mapping = master_mapping
      self.encode_data = encode_data
      self.encode_master_mapping = encode_master_mapping
      self.format_data = format_data
