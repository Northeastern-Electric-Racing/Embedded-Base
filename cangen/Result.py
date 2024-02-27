
class Result:
   """
   This class is used to store the results of the RustSynth.py script.
   decode_data is the synthesized Rust code for the decode_data.rs file.
   master_mapping is the synthesized Rust code for the master_mapping.rs file.
   """
   decode_data: str
   master_mapping: str

   def __init__(self, decode_data: str, master_mapping: str):
      self.decode_data = decode_data
      self.master_mapping = master_mapping
