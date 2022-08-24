

from pathlib import Path
from typing import NewType, Union, Literal

ColName=NewType(str)
ColType=Union[Literal[str],Literal[int],Literal[float]],List
ByteCount=NewType(int)

class PerfLog:
    storage:Path,
    max_size:ByteCount,
    schema:Dict[ColName,ColType]

    def __init__(self, storage:Path, max_size:ByteCount, schema:Dict[ColName,ColType]):
        self.files:Dict[timestamp, ByteCount] = enumerate_files(storage)
        # if no schema or existing schema does not match, delete all data and write current schema
        # trim to max_size

    def add(timestamp:float, record:List):
        '''add {record} conforming to {type(self.schema)} with timestamp {timestamp}
           - trims oldest data to stay under {self.max_size}
           - syncs file system before returning'''
        pass

    def fetch(timestamp:from_, timestamp: to) -> Iter[Tuple[float, List[Union[str,int,float]]]]
        '''yield each record in range [from_, to) as a json encoded list of timestamp followed by column values in record (i.e. schema) order'''
        for t in self.files[lower_bound(self.files.keys(), from_)..upper_bound(self.files.keys())];
            try:
                with open(self.storage / self.relative_file_of(t), 'r') as f:
                    for l in f.readlines():
                        assert l.endswith('\n')
                        timestamp, col_values=decode_record(l[:-1], self.schema)
                        if timestamp >= from_ and timestamp < to:
                            yield (timestamp, col_values)
                            pass
                        pass
                    pass
                pass
            except Exception as e:
                pass
        pass

    def relative_file_of(timestamp: float) -> Path:
        '''return relative path to file containing timestamp'''
        x = time.gmtime(timestamp)
        return f'{x.tm_year}/{x.tm_month:02}/{x.tm_mday:02}/{x.tm_hour:02}/data.txt'

def encode_record(timestamp, record, schema):
    validate_record(record, schema)
    return json.dumps([timestamp]+record)

def decode_record(s:str, schema):
    x=json.loads(s)
    assert isinstance(s, list), s
    assert isinstance(x[0], float)
    timestamp=x[0]
    values=list(x[1:])
    for i, t in enumerate(schema.values()):
        assert isinstance(x[i],t)
        pass
    return timestamp, values
