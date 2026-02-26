"""
Author: Andrew Langan
Date: 2026

This file contains helpers to write the secrets.h file

Copyright: Copyright (c) 2025
"""

import os.path
from typing import Any

SOURCE_DIR: str = os.path.dirname(os.path.abspath(__file__))
SECRETS_BASE: str = os.path.join(
    SOURCE_DIR,
    "include",
    "secrets.h.base",
)
SECRETS_OUT: str = os.path.join(
    SOURCE_DIR,
    "include",
    "secrets.h",
)
HEADER = "#pragma once\n\n// This file is auto-generated. Do not edit directly!\n\n"


class CVar:
    """
    Represents a C variable declaration, which can be written to a file in proper C syntax.
    """

    static: bool
    const: bool
    type: str
    name: str
    values: list[Any]
    array: bool

    def __init__(
        self,
        type: str,
        name: str,
        values: Any | list[Any],
    ) -> None:
        """
        Initializes a C variable representation.

        Args:
            type (str): The C type of the variable (e.g., "int", "uint8_t[]", etc.).
            name (str): The name of the variable.
            values (Any | list[Any]): The value(s) of the variable,
                which will be converted to a string representation suitable for C code.
            static (bool, optional): Whether the variable should be declared as static. Defaults to False.
            const (bool, optional): Whether the variable should be declared as const. Defaults to False.
        """

        self.array = type.find("[") != -1

        if "static" in type:
            self.static = True
            type = type.replace("static", "").strip()
        else:
            self.static = False

        if "const" in type:
            self.const = True
            type = type.replace("const", "").strip()
        else:
            self.const = False

        if "[" in type and "]" in type:
            self.array = True
            type = type.replace("[", "").replace("]", "")
        else:
            self.array = False

        self.type = type

        self.name = name

        if self.array:
            assert isinstance(
                values, list
            ), "Array variables must be initialized with a list of values."
            self.values = values.copy()
        else:
            assert not isinstance(
                values, list
            ), "Non-array variables must be initialized with a single value."
            self.values = [str(values)]

    def write(
        self,
        file: str,
    ) -> None:
        """
        Writes the variable declaration to the specified file in proper C syntax.
        The declaration will be appended to the end of the file.

        Args:
            file (str): The path to the file where the variable declaration should be written.
                The variable will be appended to the end of the file.
        """

        with open(file, "at", encoding="utf-8") as f:
            if self.static:
                f.write("static ")
            if self.const:
                f.write("const ")
            f.write(f"{self.type} {self.name}")

            if self.array:
                f.write(f"[{len(self.values)}] = {{")
                for i, v in enumerate(self.values):
                    f.write(str(v))
                    f.write(", ")
                    if i % 16 == 15:
                        f.write("\n")
                f.write("\n}")
            else:
                f.write(f" = {self.values[0]}")

            f.write(";\n\n")


class ConfigFile:
    """
    Represents a configuration file that can have C variable declarations written to it.
    The file will be created if it does not exist, and variable declarations will be appended to the end of the file.
    """

    template: str
    output: str
    vars: list[CVar]

    def __init__(
        self,
        template: str = SECRETS_BASE,
        output: str = SECRETS_OUT,
    ) -> None:
        """
        Initializes a ConfigFile instance with the specified template and output file paths.

        Args:
            template (str, optional): The path to the template file that will be used as the base for the configuration file.
                The contents of this file will be copied to the output file before any variable declarations are written. Defaults to SECRETS_BASE.
            output (str, optional): The path to the output file where the configuration will be written.
                If the file does not exist, it will be created. Variable declarations will be appended to the end of this file. Defaults to SECRETS_OUT.
        """

        self.template = template
        self.output = output
        self.vars = []

        with open(self.template, "rt", encoding="utf-8") as f:
            template_contents: str = f.read()
        with open(self.output, "wt", encoding="utf-8") as f:
            f.write(HEADER)
            f.write(template_contents)

    def add_var(
        self,
        var: CVar,
    ) -> None:
        """
        Adds a C variable declaration to the configuration file. The variable will be written to the end of the file.

        Args:
            var (CVar): The C variable declaration to add to the configuration file.
        """

        self.vars.append(var)
        var.write(self.output)
