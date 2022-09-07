import pyalpaca

# Create format string
format = '?cifs[i][[d]][3c]{c:i}{I}(cif)(s(dI))'

# Construct object
object = [
    False, 
    'a', 
    5, 
    3.14, 
    "Hello World!",
    [0, 1, 2, 3], 
    [[1.1, 2.2], [3.3, 4.4], [5.5, 6.6]],
    ['a', 'b', 'c'],
    {'a': 5, 'b': 19},
    {1, 1, 1, 2, 3, 4, 5, 5, 5, 5, 6},
    ('a', 45, 2.718),
    ("Hello", (39.456, 21))
]

# Serialize
bytes = pyalpaca.serialize(format, object)

# Print it
print("Bytes:")
hex_values = ["0x{:02x}".format(b) for b in bytes]
for i, h in enumerate(hex_values):
    if i > 0 and i % 8 == 0:
        print("\n  ", end="")
    elif i == 0 and i % 8 == 0:
        print("  ", end="")
    print(h, end=" ")
print()

# Deserialize
recovered = pyalpaca.deserialize(format, bytes)

# Print it
print("\nDeserialized:\n[ ")
for i in recovered:
    print("    " + str(i) + ",")
print("]")