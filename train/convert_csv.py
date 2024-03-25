import json
import os
import types
import zipfile

import numpy
import pandas

import csv

for filename in os.listdir():
    if filename.endswith('.dai'):
        with open(filename) as f:
            project_data = json.load(f, object_hook=lambda d: types.SimpleNamespace(**d))

segments, labels = [], []
one_hot_encoding = {}

csv_file_name = "output.csv"
with open(csv_file_name, "w", newline="") as csvfile:
    writer = csv.writer(csvfile)
    writer.writerow(["X", "Y", "Z", "Label"])

    for file_data in project_data:
        try:
            sensor_data = pandas.read_csv(file_data.file_name)

            for session in file_data.sessions:
                for segment in session.segments:
                    if segment.value not in one_hot_encoding:
                        one_hot_encoding[segment.value] = len(one_hot_encoding)
                    segment_data = sensor_data[segment.start:segment.end + 1].drop(columns=['sequence'])
                    for w in segment_data.values:
                        print(w)
                        writer.writerow([w[0], w[1], w[2], 'O'])

                    segments.append(segment_data.values.ravel())
                    labels.append(one_hot_encoding[segment.value])
        except FileNotFoundError:
            pass
