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

        if self.optional_values:
            self.optional_type = 'Optional_' + self.type + str(state.optional_sequence)
            state.optional_sequence += 1

    def writeSpec(self, file, optional_prefix = False):
        optional = 'Optional_' if optional_prefix and self.optional_values else ''

        array_string = '[' + self.array_value + ']' if self.array_value else ''
        file.write('    ' + optional + self.type + ' ' + self.name + array_string + ';\n')

    def writeSerialization(self, file, spaces, var_prefix = '', optional_prefix = False):
        name = var_prefix + self.name
        var_type = 'Optional_' + self.type if optional_prefix and self.optional_values else self.type
        endianness = self.endianness + ', ' if self.endianness else ''
        endian = 'endian_' if self.endianness else ''
        array = 'array_' if self.array_value else ''
        array_size = ', ' + self.array_value if self.array_value else ''
        direction = '&' if not State.is_basic_type(self.type) else ''

        file.write(' ' * spaces)
        file.write('serialize_' + endian + array + var_type + '(buffer, ' + endianness + direction + 'input->' + name + array_size + ');\n')

    def writeDeserialization(self, file, spaces, var_prefix = '', optional_prefix = False):
        name = var_prefix + self.name
        var_type = 'Optional_' + self.type if optional_prefix and self.optional_values else self.type
        endianness = self.endianness + ', ' if self.endianness else ''
        endian = 'endian_' if self.endianness else ''
        array = 'array_' if self.array_value else ''
        array_size = ', ' + self.array_value if self.array_value else ''
        direction = '&' if not self.array_value else ''
        aux = ', aux' if not State.is_basic_type(self.type) else ''

        file.write(' ' * spaces)
        file.write('deserialize_' + endian + array + var_type + '(buffer, ' + endianness + direction + 'output->' + name + array_size + aux + ');\n')

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
        for attribute in self.attributes:
            if attribute.optional_values:
                file.write('\n\ntypedef struct ' + attribute.optional_type + '\n{\n')
                file.write('    uint8_t optional;\n')
                attribute.writeSpec(file);
                file.write('\n}' + attribute.optional_type + ';\n')

        file.write('\n\ntypedef ' + self.struct + ' ' + self.name + '\n{\n')

        for attribute in self.attributes:
            attribute.writeSpec(file, True);

        file.write('\n} ' + self.name + ';\n')


    def writeSerializationHeader(self, file):
        for attribute in self.attributes:
            if attribute.optional_values:
                file.write('\nvoid serialize_' + attribute.optional_type + '(MicroBuffer* buffer, const ' + attribute.optional_type +
                    '* input);\n')
                file.write('void deserialize_' + attribute.optional_type + '(MicroBuffer* buffer, ' + attribute.optional_type +
                     '* output, AuxMemory* aux);\n')

        file.write('\nvoid serialize_' + self.name + '(MicroBuffer* buffer, const ' + self.name +
                 '* input);\n')
        file.write('void deserialize_' + self.name + '(MicroBuffer* buffer, ' + self.name +
                     '* output, AuxMemory* aux);\n')

    def writeSerializationImplementation(self, file):
        for attribute in self.attributes:
            if attribute.optional_values:
                file.write('\nvoid serialize_' + attribute.optional_type + '(MicroBuffer* buffer, const ' + attribute.optional_type +
                    '* input)\n{\n')
                file.write('    serialize_uint8_t(buffer, input->optional);\n')
                file.write('    switch(input->optional)\n')
                file.write('    {\n')

                for value in attribute.optional_values:
                    file.write('        case ' + value + ':\n')

                attribute.writeSerialization(file, 12)
                file.write('    }\n')
                file.write('}\n\n')


                file.write('void deserialize_' + attribute.optional_type + '(MicroBuffer* buffer, ' + attribute.optional_type +
                     '* output, AuxMemory* aux){\n')
                file.write('    deserialize_uint8_t(buffer, &output->optional);\n')
                file.write('    switch(output->optional)\n')
                file.write('    {\n')

                for value in attribute.optional_values:
                    file.write('        case ' + value + ':\n')

                attribute.writeDeserialization(file, 12)
                file.write('    }\n')
                file.write('}\n\n')


        file.write('void serialize_' + self.name + '(MicroBuffer* buffer, const ' + self.name +
                 '* input)\n{\n')

        if len(self.attributes) == 2 and self.attributes[0].name == 'size' and self.attributes[1].name == 'data':
            data_type = self.attributes[1].type[:-1]
            if State.is_basic_type(data_type):
                file.write('    serialize_sequence_' + data_type + '(buffer, input->data, input->size);\n')

            else:
                file.write('    serialize_uint32_t(buffer, input->size);\n')
                file.write('    for(uint32_t i = 0; i < input->size; i++)\n')
                file.write('        serialize_' + data_type + '(buffer, &input->data[i]);\n')

        else:
            for attribute in self.attributes:
                attribute.writeSerialization(file, 4, '', True)

        file.write('}\n\n')

        file.write('void deserialize_' + self.name + '(MicroBuffer* buffer, ' + self.name +
                 '* output, AuxMemory* aux)\n{\n')

        if len(self.attributes) == 2 and self.attributes[0].name == 'size' and self.attributes[1].name == 'data':
            data_type = self.attributes[1].type[:-1]
            if State.is_basic_type(data_type):
                file.write('    deserialize_sequence_' + data_type + '(buffer, output->data, &output->size);\n')

            else:
                file.write('    deserialize_uint32_t(buffer, &output->size);\n')
                file.write('    for(uint32_t i = 0; i < output->size; i++)\n')
                file.write('        deserialize_' + data_type + '(buffer, &output->data[i], aux);\n')

        else:
            for attribute in self.attributes:
                attribute.writeDeserialization(file, 4, '', True)

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
        file.write('void serialize_' + self.name + '(MicroBuffer* buffer, const ' + self.name +
                 '* input)\n{\n')

        self.discriminator.writeSerialization(file, 4)
        file.write('    switch(input->' + self.discriminator.name + ')\n')
        file.write('    {\n')

        for case, attribute in zip(self.discriminator_values, self.union.attributes):
            file.write('        case ' + case + ':\n')
            attribute.writeSerialization(file, 12, '_.')
            file.write('        break;\n')

        file.write('    }\n')
        file.write('}\n\n')

        file.write('void deserialize_' + self.name + '(MicroBuffer* buffer, ' + self.name +
                 '* output, AuxMemory* aux)\n{\n')

        self.discriminator.writeDeserialization(file, 4)
        file.write('    switch(output->' + self.discriminator.name + ')\n')
        file.write('    {\n')

        for case, attribute in zip(self.discriminator_values, self.union.attributes):
            file.write('        case ' + case + ':\n')
            attribute.writeDeserialization(file, 12, '_.')
            file.write('        break;\n')

        file.write('    }\n')
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
with open('xrce_protocol.yaml', 'r') as f:
    src = yaml.load(f)

microCDRGen = MicroCDRGenerator(src)

headers = '../include/micrortps/client/'
sources = '../src/c/'

microCDRGen.writeSpec(headers + 'xrce_protocol_spec.h')
microCDRGen.writeSerializationHeader(headers + 'xrce_protocol_serialization.h')
microCDRGen.writeSerializationImplementation(sources + 'xrce_protocol_serialization.c')