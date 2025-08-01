# 2025-08-01T11:18:28.420524700
import vitis

client = vitis.create_client()
client.set_workspace(path="VGGHLS-TX")

comp = client.create_hls_component(name = "TL_Base",cfg_file = ["hls_config.cfg"],template = "empty_hls_component")

