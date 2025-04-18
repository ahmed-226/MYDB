import subprocess
import os

def run_script(commands, reset_db=True):
    """
    Runs the database program with the given commands and captures the output.
    """
    
    if reset_db and os.path.exists("test.db"):
        os.remove("test.db")

    process = subprocess.Popen(
        ["./mydb.exe", "test.db"],  
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True  
    )
    
    stdout, stderr = process.communicate("\n".join(commands) + "\n")
    return stdout.splitlines()

def test_insert_and_select():
    """
    Test inserting and selecting a row from the database.
    """
    result = run_script([
        "insert 1 user1 person1@example.com",
        "select",
        ".exit",
    ])
    expected = [
        "db > Executed.",
        "db > (1, user1, person1@example.com)",
        "Executed.",
        "db > ",
    ]
    assert result == expected, f"Expected {expected}, but got {result}"

def test_table_full_error():
    """
    Test that the database prints an error message when the table is full.
    """
    script = [f"insert {i} user{i} person{i}@example.com" for i in range(1, 1402)]
    script.append(".exit")
    result = run_script(script)
    assert result[-2] == "db > Error: Table full.", f"Expected 'db > Error: Table full.', but got {result[-2]}"

def test_maximum_length_strings():
    """
    Test inserting strings that are the maximum allowed length.
    """
    long_username = "a" * 32
    long_email = "a" * 255
    script = [
        f"insert 1 {long_username} {long_email}",
        "select",
        ".exit",
    ]
    result = run_script(script)
    expected = [
        "db > Executed.",
        f"db > (1, {long_username}, {long_email})",
        "Executed.",
        "db > ",
    ]
    assert result == expected, f"Expected {expected}, but got {result}"

def test_string_too_long_error():
    """
    Test that the database prints an error message when strings are too long.
    """
    long_username = "a" * 33  
    long_email = "a" * 256    
    script = [
        f"insert 1 {long_username} {long_email}",
        "select",
        ".exit",
    ]
    result = run_script(script)
    expected = [
        "db > String is too long.",
        "db > Executed.",
        "db > ",
    ]
    assert result[:3] == expected, f"Expected {expected}, but got {result[:3]}"

def test_persistence_after_exit():
    """
    Test that the database keeps data after closing and reopening the connection.
    """
    
    result1 = run_script([
        "insert 1 user1 person1@example.com",
        ".exit",
    ], reset_db=True)  
    expected1 = [
        "db > Executed.",
        "db > ",
    ]
    assert result1 == expected1, f"Expected {expected1}, but got {result1}"

    
    result2 = run_script([
        "select",
        ".exit",
    ], reset_db=False)  
    expected2 = [
        "db > (1, user1, person1@example.com)",
        "Executed.",
        "db > ",
    ]
    assert result2 == expected2, f"Expected {expected2}, but got {result2}"

def test_print_constants():
    """
    Test that the database prints constants correctly.
    """
    script = [
        ".constants",
        ".exit",
    ]
    result = run_script(script)
    expected = [
        "db > Constants:",
        "ROW_SIZE: 293",
        "COMMON_NODE_HEADER_SIZE: 6",
        "LEAF_NODE_HEADER_SIZE: 10",
        "LEAF_NODE_CELL_SIZE: 297",
        "LEAF_NODE_SPACE_FOR_CELLS: 4086",
        "LEAF_NODE_MAX_CELLS: 13",
        "db > ",
    ]
    assert result == expected, f"Expected {expected}, but got {result}"

def test_print_btree_structure():
    """
    Test that the database prints the structure of a one-node B-tree.
    """
    script = [f"insert {i} user{i} person{i}@example.com" for i in [3, 1, 2]]
    script.append(".btree")
    script.append(".exit")
    result = run_script(script)
    expected = [
        "db > Executed.",
        "db > Executed.",
        "db > Executed.",
        "db > Tree:",
        "leaf (size 3)",
        "  - 0 : 3",
        "  - 1 : 1",
        "  - 2 : 2",
        "db > ",
    ]
    assert result == expected, f"Expected {expected}, but got {result}"

if __name__ == "__main__":
    test_insert_and_select()
    test_table_full_error()
    test_string_too_long_error()
    test_maximum_length_strings()
    test_persistence_after_exit()
    test_print_constants()
    test_print_btree_structure()
    print("All tests passed!")

    if os.path.exists("test.db"):
        os.remove("test.db")