import pandas as pd
import matplotlib.pyplot as plt
import json


# Run correction first
import correct_json

# Filter labels
df = pd.read_json("./out_corrected.json")
short_labels = ["PTS","PRS",
                "L","R","F",
                "DL","DR",
                "SL","SR",
                "OT","OR",
                "T"]
long_labels = ["T_target", "R_target",
               "Left_distance", "Right_distance", "Front_distance",
               "Left_travel", "Right_travel",
               "Left_speed", "Right_speed",
               "T_output", "R_output",
               "time"]
df = df[short_labels]
df.columns = long_labels

print(df.tail())

# plot
fig, axes = plt.subplots(nrows=3, ncols=1)
pids = df[["T_target", "R_target",
           "T_output", "R_output",
           "time"]]
speed = df[["Left_speed", "Right_speed",
            "Left_travel", "Right_travel",
            "time"]]
range_sensor = df[["Left_distance", "Right_distance", "Front_distance",
                   "Left_travel", "Right_travel",
                   "time"]]

pids.plot(x="time", ax=axes[0]) #, ylim=(0,speeds["T_setpoint"].max()+10), 
speed.plot(x="time", ax=axes[1]) #, ylim=(-10,pwms["Output_Left"].max()), 
range_sensor.plot(x="time", ax=axes[2]) #, ylim=(-10,pwms["Output_Left"].max()), 
# df.plot(secondary_y=["Output_Left", "Output_Right"], mark_right=False)
plt.show()
