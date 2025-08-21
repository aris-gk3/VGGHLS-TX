# 2025-08-22T00:10:53.612739700
import vitis

client = vitis.create_client()
client.set_workspace(path="VGGHLS-TX")

comp = client.get_component(name="TL_Base")
comp.run(operation="SYNTHESIS")

