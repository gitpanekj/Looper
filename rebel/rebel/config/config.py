import click
import yaml
import os

class UnknownConfigurationOption(Exception):
    pass

class Configuration:
    cfg_paths = []
    config = {}
    
    def __class_getitem__(cls, *compound_key):
        _option = cls.config
        for key in compound_key:
            try:
                _option = _option[key]
            except KeyError:
                raise UnknownConfigurationOption(f"{compound_key}") 
        return _option

    @classmethod
    def option(cls, *args, default=None, **kwargs):
        _config = cls.config
        for key in args[:-1]:
            _group = _config.get(key, {})
            _config[key] = _group
            _config = _group
        
        _config[args[-1]] = default
        cls.cfg_paths.append(args)
            
        
        cli_option = "-".join(['-', *args])
        return click.option(cli_option, **kwargs)
    
    @classmethod
    def load_configuration(cls, path: str, **options):
        with open(path, 'r') as config_file:
            _config = yaml.safe_load(config_file)
            for config_path in cls.cfg_paths:
                cls.copy_config_form_file(cls.config, _config, config_path)
        
        for option_name, option_value in options.items():
            if option_value:
                cls.assign_option(cls.config, tuple(option_name.split('_')), option_value)
    
    @staticmethod
    def copy_config_form_file(config, loaded_config, path):
        _option = config
        _l_option = loaded_config
        
        try:
            for key in path[:-1]:
                _option = _option[key]
                _l_option = _l_option[key]
            

            _option[path[-1]] = _l_option[path[-1]]
        except KeyError:
            return # config fiel does not contain the path, stick with defauls

    
    @staticmethod
    def assign_option(config, path, value):
        _option = config
        for key in path[:-1]:
            _option = _option[key]
        _option[path[-1]] = value