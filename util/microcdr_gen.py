# Copyright 2017 Proyectos y Sistemas de Mantenimiento SL (eProsima).
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

#!/usr/bin/env python
class State:
    def __init__(self):
        self.aliases = {}
        self.optional_sequence = 0

    def process_var_type(self, var_type_raw):
        if self.aliases.has_key(var_type_raw):
            var_type_raw = self.aliases[var_type_raw]

        var_type_splitted = var_type_raw.split()
        var_type = var_type_splitted[0]
        var_array_value = 0

        if len(var_type_splitted) == 2:
            var_array_value = var_type_splitted[1]

        return var_type, var_array_value

    @staticmethod
    def is_basic_type(var_type):
        return (var_type == 'bool' or
               var_type == 'char' or
               var_type == 'uint8_t' or
               var_type == 'uint16_t' or
               var_type == 'uint32_t' or
               var_type == 'uint64_t' or
               var_type == 'int16_t' or
               var_type == 'int32_t' or
               var_type == 'int64_t' or
               var_type == 'float' or
               var_type == 'double')

    def get_type_bytes_size(var_type):
        if var_type == 'bool':
            return 1
        if var_type == 'char':
            return 1
        if var_type == 'uint8_t':
            return 1
        if var_type == 'uint16_t':
            return 2
        if var_type == 'uint32_t':
            return 4
        if var_type == 'uint64_t':
            return 8
        if var_type == 'int16_t':
            return 2
        if var_type == 'int32_t':
            return 4
        if var_type == 'int64_t':
            return 8
        if var_type == 'float':
            return 4
        if var_type == 'double':
            return 8

class Attribute:
    def __init__(self, yaml_attribute, state):
        self.endianness = ''
        self.optional_values = []

        for name in yaml_attribute.keys():
            if name == '__ENDIANNESS__':
                self.endianness = yaml_attribute[name] + '_ENDIANNESS'
            elif name == '__OPTIONAL_CONDITION__':
                self.optional_values = yaml_attribute[name]
            else:
                self.name = name
                self.type, self.array_value = state.process_var_type(yaml_attribute[name])

        self.op_type = 'bool' if len(self.optional_values) == 1 else 'uint8_t'

    def writeSpec(self, file):
        array_string = '[' + self.array_value + ']' if self.array_value else ''
        if self.optional_values:
            file.write('    ' + self.op_type + ' optional_' + self.name + ';\n')
        file.write('    ' + self.type + ' ' + self.name + array_string + ';\n')

    def writeSerialization(self, file, spaces, var_prefix = '', optional_prefix = False):
        name = var_prefix + self.name
        endianness = self.endianness + ', ' if self.endianness else ''
        endian = 'endian_' if self.endianness else ''
        array = 'array_' if self.array_value else ''
        array_size = ', ' + self.array_value if self.array_value else ''
        direction = '&' if not State.is_basic_type(self.type) else ''

        if self.optional_values:
            file.write(' ' * spaces)
            file.write('ret &= serialize_' + self.op_type + '(buffer, input->optional_' + self.name + ');\n')
            file.write(' ' * spaces)
            file.write('if(input->optional_' + self.name + ' == ' + self.optional_values[0])
            for i in xrange(1, len(self.optional_values)):
                file.write('\n    || input->optional_' + self.name + ' == ' + self.optional_values[i])
            file.write(')\n')
            file.write(' ' * spaces)
            file.write('{\n')
            file.write(' ' * spaces)
            file.write(' ' * spaces)
            file.write('ret &= serialize_' + endian + array + self.type + '(buffer, ' + endianness + direction + 'input->' + name + array_size + ');\n')
            file.write(' ' * spaces)
            file.write('}\n')
        else:
            file.write(' ' * spaces)
            file.write('ret &= serialize_' + endian + array + self.type + '(buffer, ' + endianness + direction + 'input->' + name + array_size + ');\n')

        if self.optional_values:
            file.write('\n')

    def writeDeserialization(self, file, spaces, var_prefix = '', optional_prefix = False):
        name = var_prefix + self.name
        endianness = self.endianness + ', ' if self.endianness else ''
        endian = 'endian_' if self.endianness else ''
        array = 'array_' if self.array_value else ''
        array_size = ', ' + self.array_value if self.array_value else ''
        direction = '&' if not self.array_value else ''

        if self.optional_values:
            file.write(' ' * spaces)
            file.write('ret &= deserialize_' + self.op_type + '(buffer, &output->optional_' + self.name + ');\n')
            file.write(' ' * spaces)
            file.write('if(output->optional_' + self.name + ' == ' + self.optional_values[0])
            for i in xrange(1, len(self.optional_values)):
                file.write('\n    || output->optional_' + self.name + ' == ' + self.optional_values[i])
            file.write(')\n')
            file.write(' ' * spaces)
            file.write('{\n')
            file.write(' ' * spaces)
            file.write(' ' * spaces)
            file.write(' ' * spaces)
            file.write('ret &= deserialize_' + endian + array + self.type + '(buffer, ' + endianness + direction + 'output->' + name + array_size + ');\n')
            file.write(' ' * spaces)
            file.write('}\n')
        else:
            file.write(' ' * spaces)
            file.write('ret &= deserialize_' + endian + array + self.type + '(buffer, ' + endianness + direction + 'output->' + name + array_size + ');\n')

        if self.optional_values:
            file.write('\n')

class Define:
    def __init__(self, yaml_define, state):
        self.id = yaml_define.keys()[0]
        self.value = yaml_define.values()[0]

    def writeSpec(self, file):
        file.write('#define ' + self.id + ' ' + self.value + '\n')

    def writeSerializationHeader(self, file):
        pass

    def writeSerializationImplementation(self, file):
        pass


class Typedef:
    def __init__(self, yaml_typedef, state):
        self.name = yaml_typedef.keys()[0]
        state.aliases[self.name] = yaml_typedef.values()[0]
        self.type, self.array_value = state.process_var_type(yaml_typedef[self.name])

    def writeSpec(self, file):
        array_string = '[' + self.array_value + ']' if self.array_value else ''
        file.write('\ntypedef ' + self.type + ' ' + self.name + array_string + ';\n')

    def writeSerializationHeader(self, file):
        pass

    def writeSerializationImplementation(self, file):
        pass


class Struct:
    def __init__(self, yaml_struct, state, struct = 'struct'):
        self.name = yaml_struct.keys()[0]
        self.attributes = []
        self.struct = struct

        for attribute in yaml_struct[self.name]:
            self.attributes.append(Attribute(attribute, state))

    def writeSpec(self, file):
        file.write('\n\ntypedef ' + self.struct + ' ' + self.name + '\n{\n')

        for attribute in self.attributes:
            attribute.writeSpec(file);

        file.write('\n} ' + self.name + ';\n')


    def writeSerializationHeader(self, file):
        file.write('\nbool serialize_' + self.name + '(MicroBuffer* buffer, const ' + self.name +
                 '* input);\n')

        file.write('bool deserialize_' + self.name + '(MicroBuffer* buffer, ' + self.name +
                     '* output);\n')

    def writeSerializationImplementation(self, file):
        file.write('bool serialize_' + self.name + '(MicroBuffer* buffer, const ' + self.name +
                 '* input)\n{\n')

        if len(self.attributes) == 2 and self.attributes[0].name == 'size' and self.attributes[1].name == 'data':
            data_type = self.attributes[1].type[:-1]
            if State.is_basic_type(data_type):
                file.write('    return serialize_sequence_' + data_type + '(buffer, input->data, input->size);\n')
            else:
                file.write('    bool ret = serialize_uint32_t(buffer, input->size);\n')
                file.write('    for(uint32_t i = 0; i < input->size; i++)\n')
                file.write('    {\n')
                file.write('        ret &= serialize_' + self.attributes[1].type + '(buffer, &input->data[i]);\n')
                file.write('    }\n')
                file.write('    return ret;\n')

        else:
            file.write('    bool ret = true;\n')
            for attribute in self.attributes:
                attribute.writeSerialization(file, 4)
            file.write('    return ret;\n')

        file.write('}\n\n')


        file.write('bool deserialize_' + self.name + '(MicroBuffer* buffer, ' + self.name +
                 '* output)\n{\n')

        if len(self.attributes) == 2 and self.attributes[0].name == 'size' and self.attributes[1].name == 'data':
            data_type = self.attributes[1].type[:-1]

            if State.is_basic_type(data_type):
                file.write('    return deserialize_sequence_' + data_type + '(buffer, &output->data, &output->size);\n')

            else:
                file.write('    bool ret = deserialize_uint32_t(buffer, &output->size);\n')
                file.write('    if (ret)\n')
                file.write('    {\n')
                file.write('        for(uint32_t i = 0; i < output->size; i++)\n')
                file.write('        {\n')
                file.write('            ret &= deserialize_' + self.attributes[1].type + '(buffer, &output->data[i], aux);\n')
                file.write('        }\n')
                file.write('    }\n')
                file.write('    return ret;\n')

        else:
            file.write('    bool ret = true;\n')
            for attribute in self.attributes:
                attribute.writeDeserialization(file, 4)
            file.write('    return ret;\n')

        file.write('}\n\n')


class Union:
    def __init__(self, yaml_union_base, state):
        self.attributes = []

        for name in yaml_union_base.keys():
            if name == '__DISCRIMINATOR__':
                self.discriminator_values = yaml_union_base[name]
            else:
                self.name = name
                self.discriminator = Attribute(yaml_union_base[name][0], state)

        yaml_struct = {}
        yaml_struct[self.name] = []
        yaml_struct[self.name].append(yaml_union_base[self.name][0])
        yaml_struct[self.name].append({'_': self.name + 'U'})
        self.struct = Struct(yaml_struct, state)

        yaml_union = yaml_union_base.copy()
        yaml_union.pop('__DISCRIMINATOR__')
        yaml_union[self.name].pop(0)
        yaml_union[self.name + 'U'] = yaml_union.pop(self.name)
        self.union = Struct(yaml_union, state, 'union')

    def writeSpec(self, file):
        self.union.writeSpec(file)
        self.struct.writeSpec(file)

    def writeSerializationHeader(self, file):
        self.struct.writeSerializationHeader(file)

    def writeSerializationImplementation(self, file):
        file.write('bool serialize_' + self.name + '(MicroBuffer* buffer, const ' + self.name +
                 '* input)\n{\n')

        file.write('    bool ret = true;\n')
        self.discriminator.writeSerialization(file, 4)
        file.write('    if (ret)\n')
        file.write('    {\n')
        file.write('        switch(input->' + self.discriminator.name + ')\n')
        file.write('        {\n')

        for case, attribute in zip(self.discriminator_values, self.union.attributes):
            file.write('            case ' + case + ':\n')
            attribute.writeSerialization(file, 16, '_.')
            file.write('                break;\n')

        file.write('            default:\n')
        file.write('                break;\n')
        file.write('        }\n')
        file.write('    }\n')
        file.write('    return ret;\n')
        file.write('}\n\n')


        file.write('bool deserialize_' + self.name + '(MicroBuffer* buffer, ' + self.name +
                 '* output)\n{\n')

        file.write('    bool ret = true;\n')
        self.discriminator.writeDeserialization(file, 4)
        file.write('    if (ret)\n')
        file.write('    {\n')
        file.write('        switch(output->' + self.discriminator.name + ')\n')
        file.write('        {\n')

        for case, attribute in zip(self.discriminator_values, self.union.attributes):
            file.write('            case ' + case + ':\n')
            attribute.writeDeserialization(file, 16, '_.')
            file.write('                break;\n')

        file.write('            default:\n')
        file.write('                break;\n')
        file.write('        }\n')
        file.write('    }\n')
        file.write('    return ret;\n')
        file.write('}\n\n')


class Enum:
    def __init__(self, yaml_enum, state):
        self.type = 'uint32_t'
        self.values = []

        for name in yaml_enum.keys():
            if name == '__TYPE__':
                self.type, null = state.process_var_type(yaml_enum[name])
            else:
                self.name = name
                for value in yaml_enum[name]:
                    self.values.append((value.keys()[0], value.values()[0]))

        state.aliases[self.name] = self.type

    def writeSpec(self, file):
        file.write('\n\ntypedef enum ' + self.name + '\n{\n')

        i = 0
        for key, value in self.values:
            file.write('    ' + key + ' = ' + value)
            if i != len(self.values) - 1:
                file.write(',')
            file.write('\n')
            i = i + 1

        file.write('\n} ' + self.name + ';\n')

    def writeSerializationHeader(self, file):
        pass

    def writeSerializationImplementation(self, file):
        pass

class MicroCDRGenerator:
    def __init__(self, yaml):
        self.state = State()

        self.spec_file_head = yaml['FILES']['spec_file']['head']
        self.h_file_head = yaml['FILES']['h_file']['head']
        self.c_file_head = yaml['FILES']['c_file']['head']

        self.spec_file_tail = yaml['FILES']['spec_file']['tail']
        self.h_file_tail = yaml['FILES']['h_file']['tail']
        self.c_file_tail = yaml['FILES']['c_file']['tail']

        self.entries = []
        for node in yaml['DATA']:
            if node.has_key('define'):
                self.entries.append(Define(node['define'], self.state))

            elif node.has_key('typedef'):
                self.entries.append(Typedef(node['typedef'], self.state))

            elif node.has_key('struct'):
                self.entries.append(Struct(node['struct'], self.state))

            elif node.has_key('union'):
                self.entries.append(Union(node['union'], self.state))

            elif node.has_key('enum'):
                self.entries.append(Enum(node['enum'], self.state))


    def writeSpec(self, filePath):
        file = open(filePath, 'w')
        file.write(self.spec_file_head +'\n')

        for entry in self.entries:
            entry.writeSpec(file)

        file.write('\n' + self.spec_file_tail)
        file.close()


    def writeSerializationHeader(self, filePath):
        file = open(filePath, 'w')
        file.write(self.h_file_head +'\n')

        for entry in self.entries:
            entry.writeSerializationHeader(file)

        file.write('\n' + self.h_file_tail)
        file.close()


    def writeSerializationImplementation(self, filePath):
        file = open(filePath, 'w')
        file.write(self.c_file_head +'\n')

        for entry in self.entries:
            entry.writeSerializationImplementation(file)

        file.write('\n' + self.c_file_tail)
        file.close()

import yaml
import sys

if len(sys.argv) == 4:
    with open(sys.argv[1], 'r') as f:
        src = yaml.load(f)

    microCDRGen = MicroCDRGenerator(src)

    file_path_points = sys.argv[1].split('.')
    file_path_slash = file_path_points[len(file_path_points) - 2].split('/')
    name = file_path_slash[len(file_path_slash) - 1]
    headers = sys.argv[2] + ('/' if sys.argv[2][-1] != '/' else '')
    sources = sys.argv[3] + ('/' if sys.argv[3][-1] != '/' else '')

    microCDRGen.writeSpec(headers + name + '_spec.h')
    microCDRGen.writeSerializationHeader(headers + name + '_serialization.h')
    microCDRGen.writeSerializationImplementation(sources + name + '_serialization.c')

else:
    print('program yaml_source headers_dir sources_dir')
