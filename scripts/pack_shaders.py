import os
from string import Template
import argparse


parser = argparse.ArgumentParser(
    description="Python shader packing script for Islands")

parser.add_argument("source_dir", action="store", type=str)
parser.add_argument("output_file", action="store", type=str)

args = parser.parse_args()

source_dir: str = args.source_dir

source_dir = os.fsencode(source_dir)

shaders_root_dir = os.path.basename(source_dir)

files: dict[str, list[str]] = dict()
deps = []


def get_paths(dir):
    with os.scandir(dir) as it:
        for entry in it:
            if entry.is_file():
                if dir not in files:
                    files[dir] = []
                deps.append(os.fsdecode(entry.path))
                files[dir].append(entry.path)
            elif entry.is_dir():
                get_paths(os.fsencode(entry.path))


get_paths(source_dir)


source: list[str] = []

for key, value in files.items():
    vars: str = ''
    for path in value:
        base_path = os.path.basename(path)
        name = os.fsdecode(base_path.upper()).replace(
            "\\", "/").replace("/", "_").replace(".", "_")

        with open(path) as sh:
            shader_contents = ''
            for line in sh:
                if line.strip() == '':
                    continue
                if line.lstrip().startswith("//"):
                    continue
                shader_contents += line

        variable_def = """
            inline constexpr const char* {var_name} = R";;(
                    {var_value});;";
        """.format(var_name=name, var_value=shader_contents)
        vars = vars + variable_def
        # print(vars)

    base = os.path.commonpath([source_dir, path])
    base = os.path.relpath(path, base)
    ns = os.fsdecode(os.path.dirname(base)).replace("/", "::")
    if ns != '':
        namespace = """
            namespace {ns} {{
                {variable_def}
            }}
        """.format(ns=ns, variable_def=vars)
        source.append(namespace)
    else:
        source.append(vars)


out_template = Template(
    """
    /* PYTHON AUTOGEN */
    namespace shaders {
        $source
    }
    """
)

source = ''.join([source for source in source])

autogen = out_template.substitute(source=source)

with open(args.output_file, "w") as out:
    out.write(autogen)
