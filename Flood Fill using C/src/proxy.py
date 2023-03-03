import argparse
import subprocess
import sys

# Keeps track of whether or not the robot has crashed and needs to reset
state = {
    "crashed" : False,
    "hasLimit" : False,
    "hasOut" : False
}

invalid_commands_for_crash = ["moveForward", "turnLeft", "turnRight", "wallFront",
                     "wallLeft", "wallRight"]

def write(stream, string):
    """Writes the string to the stream and then flushes the stream"""
    stream.write(string)
    stream.flush()

def invalid_to_send(line):
    """Checks if the line is invalid to send, either because the mouse is
    crashed or because the command is getStat. Returns a response to the
    algorithm in place of sending the command to the simulator."""
    if state["crashed"]:
        # Commands to move, turn, or detect walls will be ignored.
        if line.split()[0] in invalid_commands_for_crash:
            write(sys.stderr, "Cannot move or detect walls while crashed. Must send ackReset.\n")
            return b"crash\n"
    # getStat command cannot be sent
    if line.split()[0] == "getStat":
        write(sys.stderr, "Competition program cannot call getStat.\n")
        return b"-1\n"
    return False

commands_that_need_response = [
    "mazeWidth",
    "mazeHeight",
    "wallFront",
    "wallRight",
    "wallLeft",
    "moveForward",
    "turnRight",
    "turnLeft",
    "wasReset",
    "ackReset",
]

def needs_response(line):
    """Takes the input line and returns True if the line needs a response from
    the simulator, otherwise returns false."""
    line_start = line.split()[0]
    return line_start in commands_that_need_response

def is_movement_command(line):
    """Takes the input line and returns True if the command was a move or turn,
    otherwise returns false."""
    line_start = line.split()[0]
    return line_start in ["moveForward", "turnRight", "turnLeft"]

def get_distance_plus_turns():
    """Gets the total effective distance plus total turns from the simulator"""
    write(sys.stdout, "getStat total-effective-distance\n")
    total_distance = float(sys.stdin.readline())
    write(sys.stdout, "getStat total-turns\n")
    total_turns = int(sys.stdin.readline())
    return total_distance + total_turns

def write_all_stats(command, excepted=False):
    """Gets relevant stats from the simulator and logs them to the output file
    """
    with open(state["outfile"], "a") as f:
        f.write("command: {}\n".format(command))
        if excepted:
            f.write("Exception occurred.\n")
        # Print several important stats
        stats_to_print = ["score",
                        "best-run-distance",
                        "best-run-effective-distance",
                        "best-run-turns",
                        "total-distance",
                        "total-effective-distance",
                        "total-turns"
                        ]
        for stat in stats_to_print:
            write(sys.stdout, "getStat {}\n".format(stat))
            value = sys.stdin.readline()
            f.write("{}: {}".format(stat, value))
        f.write("\n")

    
def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out", help="file to output the score")
    parser.add_argument("--limit", type=int, help="maximum number of turns plus movements before ending program")

    parser.add_argument("command", type=str, nargs="+", help="command that executes the maze-solving program")
    args = vars(parser.parse_args())
    write(sys.stderr, " ".join(args['command'])+'\n')

    # Determine if a limit is invoked
    if args["limit"] is not None:
        state["hasLimit"] = True
        state["limit"] = args["limit"]
    # Determine if an output log is requested
    if args["out"] is not None:
        state["hasOut"] = True
        state["outfile"] = args["out"]

    # Launch the subprocess
    proc = subprocess.Popen(args['command'], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
        
    try:
        # Loop until subprocess terminates
        for line in iter(proc.stdout.readline, b''):
            decode_line = line.decode('utf-8')
            response = invalid_to_send(decode_line)
            if response:
                # Send "crash" if mouse had previously crashed and has not reset.
                # Send "-1" if getStat was called
                write(proc.stdin, response)
                continue
            write(sys.stdout, decode_line)
            
            if (needs_response(decode_line)):
                inp = sys.stdin.readline() # Read from simulator
                if (inp == "crash\n"):
                    state["crashed"] = True
                elif (decode_line == "ackReset\n" and inp == "ack\n"):
                    # No longer crashed
                    state["crashed"] = False
                write(proc.stdin, inp.encode('utf-8'))

                # Check if mouse exceeded limit of moves + turns
                if (is_movement_command(decode_line) and state["hasLimit"]
                    and get_distance_plus_turns() > state["limit"]):
                    write(sys.stderr, "Maximum movements exceeded\n")
                    proc.kill()
                    break
        # Process is complete, log stats.
        if state["hasOut"]:
            write_all_stats(" ".join(args['command']))
    except:
        write(sys.stderr, "exception\n")
        if state["hasOut"]:
            write_all_stats(" ".join(args['command']), excepted=True)
        proc.kill()
        raise

if __name__ == "__main__":
    main()
