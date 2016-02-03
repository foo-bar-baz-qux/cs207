import os
from collections import defaultdict
import itertools
import reprlib

def lc_reader(filename):
    lclist=[]
    with open(filename) as fp:
        lCnt = 1
        headers = [None]*2
        for line in fp:
            if lCnt <= 2:
                headers[lCnt-1] = [str(h) for h in line[1:].strip().split()]
            elif line.find('#')!=0:
                lclist.append([float(f) for f in line.strip().split()])

            lCnt += 1

    return lclist, dict(zip(headers[0], headers[1]))


class LightCurve:

    def __init__(self, data, metadict):
        self._time = [x[0] for x in data]
        self._amplitude = [x[1] for x in data]
        self._error = [x[2] for x in data]
        self.metadata = metadict
        self.filename = None

    @classmethod
    def from_file(cls, filename):
        lclist, metadict = lc_reader(filename)
        instance = cls(lclist, metadict)
        instance.filename = filename
        return instance

    def __repr__(self):
        class_name = type(self).__name__
        class_name = 'foo'
        components = reprlib.repr(list(itertools.islice(self.timeseries,0,10)))
        components = components[components.find('['):]
        return '{}({})'.format(class_name, components)

    #your code here
    @property
    def amplitude(self):
        return self._amplitude

    @property
    def time(self):
        return self._time

    @property
    def timeseries(self):
        return zip(self._time, self._amplitude)

    #sequence protocol
    def __len__(self):
        return len(self._time)

    def __getitem__(self, pos):
        return (self._time[pos], self._amplitude[pos])



class LightCurveDB:

    def __init__(self):
        self._collection={}
        self._field_index=defaultdict(list)
        self._tile_index=defaultdict(list)
        self._color_index=defaultdict(list)

    def populate(self, folder):
        for root,dirs,files in os.walk(folder): # DEPTH 1 ONLY
            for file in files:
                if file.find('.mjd')!=-1:
                    the_path = root+"/"+file
                    self._collection[file] = LightCurve.from_file(the_path)

    def index(self):
        for f in self._collection:
            lc, tilestring, seq, color, _ = f.split('.')
            field = int(lc.split('_')[-1])
            tile = int(tilestring)
            self._field_index[field].append(self._collection[f])
            self._tile_index[tile].append(self._collection[f])
            self._color_index[color].append(self._collection[f])

    #your code here
    def retrieve(self, facet, value):
        fNames = ['field', 'tile', 'color']
        lookD = [self._field_index, self._tile_index, self._color_index]
        facets = dict(zip(fNames, lookD))

        if facet not in facets or value not in facets[facet]:
            return []

        return facets[facet][value]
