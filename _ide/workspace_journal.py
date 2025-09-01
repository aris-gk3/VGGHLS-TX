# 2025-09-01T16:14:30.768894100
import vitis

client = vitis.create_client()
client.set_workspace(path="VGGHLS-TX")

comp = client.get_component(name="TL_Base")
comp.run(operation="C_SIMULATION")

comp.run(operation="C_SIMULATION")

comp.run(operation="C_SIMULATION")

comp.run(operation="C_SIMULATION")

comp.run(operation="C_SIMULATION")

comp.run(operation="C_SIMULATION")

comp.run(operation="C_SIMULATION")

comp.run(operation="SYNTHESIS")

comp.run(operation="CO_SIMULATION")

