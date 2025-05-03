from abc import ABC, abstractmethod
from typing import override
import yaml
from pathlib import Path
import os
from rebel.config import Configuration

class IDictStorageManager(ABC):
    @abstractmethod
    def __init__(self, filename: str) -> None:
        pass
    
    @abstractmethod
    def write(self, dct: dict) -> None:
        pass

class YAMLStorageManager(IDictStorageManager):
    @override
    def __init__(self, filename: str) -> None:
        self.file = open(filename, 'a')
        self.results = {}
        self.result_cache_size = 0
    
    @override
    def write(self, dct: dict) -> None:
        self.results.update(dct)
        if len(self.results) > self.result_cache_size:
            self.save_results_to_yaml(self.file, self.results)
            self.results.clear()
    
    @staticmethod
    def save_results_to_yaml(file, results):
        serialized_results = yaml.dump(results)
        file.write(serialized_results)
            
    def __del__(self):
        if len(self.results) > 0:
            self.save_results_to_yaml(self.filename, self.results)
        self.file.close()

class Logger:
    def __init__(self) -> None:
        self.directory = None
        self.file = None
        self.cache = []
        self.result_cache_size = 0
             
    def set_output_path(self, directory, filename='log.txt'):
        self.directory = directory
        
        if Configuration['logging']:
            if self.file and not self.file.closed:
                self.file.close()
            self.file = open(Path(directory) / Path(filename), 'a')
        
    def log(self, message) -> None:
        if Configuration['logging']:
            self.cache.append(message + "\n")
            if len(self.cache) > self.result_cache_size:
                self.save_to_txt(self.file, self.cache)
                self.cache.clear()
    
    def error(self, message) -> None:
        self.log(f"ERROR: {message}")
    
    def info(self, message) -> None:
        self.log(f"INFO: {message}")
    
    def warning(self, message) -> None:
        self.log(f"WARNING: {message}")
    
    
    def save_in_directory(self, filename, content):
        with open(Path(self.directory) / Path(filename), "w") as f:
            f.write(content)
    
    @staticmethod
    def save_to_txt(file, results):
        file.writelines(results)
            
    def __del__(self):
        if self.file:
            if len(self.cache) > 0:
                self.save_to_txt(self.file, self.cache)
                self.cache.clear()
            self.file.close()

analysis_logger = Logger()