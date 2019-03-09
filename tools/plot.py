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
df["travel_distance"] = (df["Left_travel"] + df["Right_travel"]) / 2

print(df.tail())

# plot
fig, axes = plt.subplots(nrows=3, ncols=2)

# Plot data over time
df.plot(
    y=["T_target", "R_target", "T_output", "R_output"],
    x="time",
    ax=axes[0, 0],
    # ylim=(0,speeds["T_setpoint"].max()+10),
    )
df.plot(
    y=["Left_speed", "Right_speed"],
    x="time",
    ax=axes[1, 0],
    # ylim=(-10,pwms["Output_Left"].max()),
    )
df.plot(
    y=["Left_distance",
        "Right_distance", "Front_distance"],
    x="time",
    ax=axes[2, 0],
    # ylim=(-10,pwms["Output_Left"].max())
    )

# Plot data over distance
df.plot(
    y=["T_target", "R_target",
        "T_output", "R_output"],
    x="travel_distance",
    ax=axes[0, 1],
    # ylim=(0,speeds["T_setpoint"].max()+10),
    )
df.plot(
    y=["Left_speed", "Right_speed"],
    x="travel_distance",
    ax=axes[1, 1],
    # ylim=(-10,pwms["Output_Left"].max()),
    )
df.plot(
    y=["Left_distance", "Right_distance", "Front_distance"],
    x="travel_distance",
    ax=axes[2, 1],
    # ylim=(-10,pwms["Output_Left"].max())
    )




plt.show()
