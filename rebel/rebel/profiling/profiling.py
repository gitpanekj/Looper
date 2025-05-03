from __future__ import annotations
from time import perf_counter_ns
from dataclasses import dataclass, field
import yaml
from rebel.config import Configuration

NANOSECONDS_IN_MILISECOND = 1000_000

@dataclass
class GroupProfilingRecord:
    group_name: str
    functions: dict[str, FunctionProfilingRecord] = field(default_factory=dict)
    
    def to_dict(self):
        return  dict([f.to_dict() for f in self.functions.values()])

    def reset(self):
        for function in self.functions.values():
            function.reset()

@dataclass
class FunctionProfilingRecord:
    function_name: str
    total_time_taken_in_nanoseconds: int
    number_of_calls: int
    
    def reset(self):
        self.total_time_taken_in_nanoseconds = 0
        self.number_of_calls = 0
    
    def to_dict(self):
        if self.number_of_calls == 0:
            return (self.function_name, {'total_time_taken_in_miliseconds': 0,
                                         'number_of_calls': 0,
                                         'average_time_taken_in_miliseconds': 0})
        return (self.function_name, {'total_time_taken_in_miliseconds': self.total_time_taken_in_nanoseconds/NANOSECONDS_IN_MILISECOND,
                                    'number_of_calls': self.number_of_calls,
                                    'average_time_taken_in_miliseconds': self.total_time_taken_in_nanoseconds/NANOSECONDS_IN_MILISECOND/self.number_of_calls})

class ProfilingManager:
    watched_functions: dict[str, GroupProfilingRecord] = {}
    
    @classmethod
    def watch(cls, group: str):
        # Add a dict for a group if not already created
        group_records = cls.watched_functions.get(group, GroupProfilingRecord(group))
        cls.watched_functions[group] = group_records
        
        def enable_watch(watch: bool = True):
            def decorator(func):
                # Return function itself if profiling is off
                if not watch:
                    return func
                
                # Add a dict for a function in a group if not already created
                function_record = cls.watched_functions[group].functions.get(func.__name__, FunctionProfilingRecord(func.__name__,0,0))
                cls.watched_functions[group].functions[func.__name__] = function_record
                
                def wrapper(*args, **kwargs):
                    if not Configuration['profiling']:
                        return func(*args, **kwargs)
                    
                    start = perf_counter_ns()
                    rv = func(*args, **kwargs)
                    td_in_nanoseconds = perf_counter_ns() - start
                    cls.watched_functions[group].functions[func.__name__].total_time_taken_in_nanoseconds += td_in_nanoseconds
                    cls.watched_functions[group].functions[func.__name__].number_of_calls += 1
                    return rv
                
                return wrapper
            return decorator
        return enable_watch
    
    @classmethod
    def save(cls, filename):
        with open(filename, 'w') as f:
            yaml.dump({name: record.to_dict() for name, record in cls.watched_functions.items()}, f)
    
    
    @classmethod
    def reset(cls):
        for group in cls.watched_functions.values():
            group.reset()