#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file


@test "Example: check ls runs without errors" {
    run ./dsh <<EOF                
ls
EOF

    # Assertions
    [ "$status" -eq 0 ]
}


@test "Can pipe up to the limit" {
    run "./dsh" <<EOF                
echo "hello" | cat | cat | cat | cat | cat | cat | cat
EOF

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    # Expected output with all whitespace removed for easier matching
    expected_output="hellolocalmodedsh4>dsh4>cmdloopreturned0"

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]

    # Assertions
    [ "$status" -eq 0 ]
}

@test "Can't pipe above limit" {
    run "./dsh" <<EOF                
echo "hello" | cat | cat | cat | cat | cat | cat | cat | cat
EOF

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    # Expected output with all whitespace removed for easier matching
    expected_output="localmodedsh4>error:pipinglimitedto8commandsdsh4>cmdloopreturned0"

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]

    # Assertions
    [ "$status" -eq 0 ]
}

@test "Basic file redirection" {
    run "./dsh" <<EOF                
echo "test" > test.txt
cat test.txt
rm test.txt
EOF

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    # Expected output with all whitespace removed for easier matching
    expected_output="testlocalmodedsh4>dsh4>dsh4>dsh4>cmdloopreturned0"

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]

    # Assertions
    [ "$status" -eq 0 ]
}

@test "Basic file appending" {
    run "./dsh" <<EOF                
echo "test" > test.txt
echo "test" >> test.txt
cat test.txt
rm test.txt
EOF

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    # Expected output with all whitespace removed for easier matching
    expected_output="testtestlocalmodedsh4>dsh4>dsh4>dsh4>dsh4>cmdloopreturned0"

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]

    # Assertions
    [ "$status" -eq 0 ]
}

@test "exit works" {
    run "./dsh" <<EOF
exit
EOF

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    # Expected output with all whitespace removed for easier matching
    expected_output="localmodedsh4>cmdloopreturned0"

    # These echo commands will help with debugging and will only print
    # if the test fails
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]
}

@test "Handles nested quotes" {
    run "./dsh" <<EOF
echo "This is 'a test' with nested quotes"
EOF

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    # Expected output with all whitespace removed for easier matching
    expected_output="Thisis'atest'withnestedquoteslocalmodedsh4>dsh4>cmdloopreturned0"

    # These echo commands will help with debugging and will only print
    # if the test fails
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]
}

@test "Handles empty command" {
    run "./dsh" <<EOF

EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="localmodedsh4>warning:nocommandsprovideddsh4>cmdloopreturned0"

    echo "Captured stdout:" 
    echo "Output: $stripped_output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    # Check if the shell exits correctly and the output is empty
    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

# not using special exit codes anymore
@test "Handles invalid command" {
    run "./dsh" <<EOF
foobar
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')

    # Expected output with all whitespace removed for easier matching
    expected_output="execvp:Nosuchfileordirectorylocalmodedsh4>localmodedsh4>dsh4>cmdloopreturned0"
    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]
}

@test "Handles permission errors" {
    run "./dsh" <<EOF
echo "hi" > test
chmod 000 test
cat test
rm -f test
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')

    # Expected output with all whitespace removed for easier matching
    expected_output="cat:test:Permissiondeniedlocalmodedsh4>dsh4>dsh4>dsh4>dsh4>cmdloopreturned0"
    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]
}

@test "dragon" {
    run "./dsh" <<EOF
dragon
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')

    # Expected output with all whitespace removed for easier matching
    expected_output="localmodedsh4>@%%%%%%%%%%%%%%%%%%%%%%%%@%%%%%%%%%%%%%%%%%%%%%%%%%%%%@%%%%%%%%%%%%@%%%%%%@%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%@@%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%@%%%%%%@%%%%%%%%@%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%@%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%@%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%@%%%%%%%%%%%%%%%%%%%%@%%%%%%%%%%%%%%@%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%@%%@%%%%%%%%%%%%@%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%@%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%@@%%%%%%%%%%%%%%%%%%%%%%%%%%@%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%@%%@%%%%%%%%%%%%%%%%@%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%@%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%@%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%@%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%@%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%@%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%@%%%%%%%%%%%%%%%%%%%%%%%%%%%%%@%@%@%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%@%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%@%%%%%%%@dsh4>cmdloopreturned0"
    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]
}

@test "exit from remote shell shuts down server" {
    # Start server in background
    ./dsh -s &
    SERVER_PID=$!
    sleep 1
        run ./dsh -c <<EOF
exit
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="socketclientmode:addr:127.0.0.1:1234rsh>cmdloopreturned0"
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]

}

@test "basic command" {
    # Start server in background
    ./dsh -s &
    SERVER_PID=$!
    sleep 1
        run ./dsh -c <<EOF
ls
exit
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]' | tr -cd '[:print:]')
    expected_output="socketclientmode:addr:127.0.0.1:1234rsh>socketservermode:addr:0.0.0.0:1234->Single-ThreadedModeStartingtohandleclientrequests...batsdragon.cdragon_data.hdragon.gzdragon.hdshdsh_cli.cdshlib.cdshlib.hmakefilersh_cli.crshlib.hrsh_server.crsh>cmdloopreturned0"
    
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} ---> ${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]

}


@test "piping command over network" {
    # Start server in background
    ./dsh -s &
    SERVER_PID=$!
    sleep 1
        run ./dsh -c <<EOF
ls | wc
exit
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]' | tr -cd '[:print:]')
    expected_output="socketclientmode:addr:127.0.0.1:1234rsh>socketservermode:addr:0.0.0.0:1234->Single-ThreadedModeStartingtohandleclientrequests...1313120rsh>cmdloopreturned0"
    
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} ---> ${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]

}

@test "command with args over network" {
    # Start server in background
    ./dsh -s &
    SERVER_PID=$!
    sleep 1
        run ./dsh -c <<EOF
echo "hi"
exit
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]' | tr -cd '[:print:]')
    expected_output="socketclientmode:addr:127.0.0.1:1234rsh>socketservermode:addr:0.0.0.0:1234->Single-ThreadedModeStartingtohandleclientrequests...hirsh>cmdloopreturned0"
    
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} ---> ${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]
}

@test "No server, no connection" {
    # Start server in background

    run ./dsh -c <<EOF

EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]' | tr -cd '[:print:]')
    expected_output="connect:Connectionrefusedstartclient:Invalidargumentsocketclientmode:addr:127.0.0.1:1234cmdloopreturned-52"
    
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} ---> ${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]
}

@test "cd works over network" {
    # Start server in background
    ./dsh -s &
    SERVER_PID=$!
    sleep 1
        run ./dsh -c <<EOF
cd ..
pwd
exit
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]' | tr -cd '[:print:]')
    expected_output="socketclientmode:addr:127.0.0.1:1234rsh>rsh>socketservermode:addr:0.0.0.0:1234->Single-ThreadedModeStartingtohandleclientrequests.../home/droch/Winter/CS283/SysProg-Class/assignments/6-RShellrsh>cmdloopreturned0"
    
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} ---> ${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]
}

@test "invalid command over network" {
    # Start server in background
    ./dsh -s &
    SERVER_PID=$!
    sleep 1
        run ./dsh -c <<EOF
foobar
exit
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]' | tr -cd '[:print:]')
    expected_output="socketclientmode:addr:127.0.0.1:1234rsh>socketservermode:addr:0.0.0.0:1234->Single-ThreadedModeStartingtohandleclientrequests...rdsh-error:commandexecutionerrorrsh>cmdloopreturned0"
    
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} ---> ${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]
}