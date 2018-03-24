import pandas as pd
import matplotlib.pyplot as plt
import json


# Run correction first
import correct_json

# Filter labels
df = pd.read_json("./out_corrected.json")
short_labels = ["LS", "RS", "LO", "RO", "TTS", "TRS"]
long_labels = ["Speed_Left", "Speed_Right", "Output_Left", "Output_Right", "T_setpoint", "R_setpoint"]
df = df[short_labels]
df.columns = long_labels
 
print(df.tail())

# plot
fig, axes = plt.subplots(nrows=2, ncols=1)
speeds = df[["Speed_Left", "Speed_Right", "T_setpoint", "R_setpoint"]]
pwms = df[["Output_Left", "Output_Right"]]  

speeds.plot(ax=axes[0]) #, ylim=(0,speeds["T_setpoint"].max()+10), 
pwms.plot(ax=axes[1]) #, ylim=(-10,pwms["Output_Left"].max()), 
# df.plot(secondary_y=["Output_Left", "Output_Right"], mark_right=False)
plt.show()





import matplotlib.pyplot as plt
import numpy as np
from pandas import DataFrame
df = DataFrame(np.random.randn(5, 3), columns=['A', 'B', 'C'])

fig, ax = plt.subplots()
ax3 = ax.twinx()
rspine = ax3.spines['right']
rspine.set_position(('axes', 1.15))
ax3.set_frame_on(True)
ax3.patch.set_visible(False)
fig.subplots_adjust(right=0.7)

df.A.plot(ax=ax, style='b-')
# same ax as above since it's automatically added on the right
df.B.plot(ax=ax, style='r-', secondary_y=True)
df.C.plot(ax=ax3, style='g-')

# add legend --> take advantage of pandas providing us access
# to the line associated with the right part of the axis
ax3.legend([ax.get_lines()[0], ax.right_ax.get_lines()[0], ax3.get_lines()[0]],\
           ['A','B','C'], bbox_to_anchor=(1.5, 0.5))