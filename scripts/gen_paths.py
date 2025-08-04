import os
from string import Template
import argparse


parser = argparse.ArgumentParser(
    description="Python codegen script for Islands game data")

parser.add_argument("source_dir", action="store", type=str)
parser.add_argument("output_file", action="store", type=str)
parser.add_argument("--deps", action="store", dest="deps", type=str)

args = parser.parse_args()


game_data_path: str = args.source_dir

game_data_dir = os.fsencode(game_data_path)

files: dict[str, list[str]] = dict()
deps = []


def get_paths(dir):
    with os.scandir(dir) as it:
        for entry in it:
            if entry.is_file():
                if dir not in files:
                    files[dir] = []
                deps.append(os.fsdecode(entry.path))
                files[dir].append(os.path.basename(entry.path))
            elif entry.is_dir():
                get_paths(os.fsencode(entry.path))


get_paths(game_data_dir)


source: list[str] = []

for key, value in files.items():
    vars: str = ''
    for path in value:
        name = os.fsdecode(path.upper()).replace("/", "_").replace(".", "_")
        variable_def = """
            inline constexpr const char* {var_name} = "{var_value}";
        """.format(var_name=name, var_value=os.path.join(os.fsdecode(key),
                                                         os.fsdecode(path)))
        vars = vars + variable_def
        # print(vars)

    ns = os.fsdecode(key).replace("/", "::")
    namespace = """
        namespace {ns} {{
            {variable_def}
        }}
    """.format(ns=ns, variable_def=vars)
    source.append(namespace)

out_template = Template(
    """
    /* PYTHON AUTOGEN */
    namespace files {
        $source
    }
    """
)

source = ''.join([source for source in source])

autogen = out_template.substitute(source=source)

with open(args.output_file, "w") as out:
    out.write(autogen)

if args.deps is not None:
    with open(args.deps, "w") as out:
        for dep in deps:
            out.write(dep+' ')
