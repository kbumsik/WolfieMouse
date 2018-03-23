import json
import os

f = open('out.json')
out = open('out_corrected.json', 'wb')

out.write(b'[\n')
for line in f.readlines():
    # Trim trailing '\n' and ','
    line = line[:-1]
    if line[-1] == ',':
        line = line[:-1]

    try:
        j = json.loads(line)
    except:
        continue

    out.write(line.encode('utf-8') + b',\n')

out.seek(-2, os.SEEK_CUR)
out.write(b'\n]')

f.close()
out.close()
