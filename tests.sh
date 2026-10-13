# Runs make clean, removes old results, makes all, runs tests and makes graphs

# Run make clean, remove old results, make all
function clean_make {
    make clean
    if [ $? -ne 0 ]; then
        echo "make clean failed"
        exit 1
    fi

    # rm results/*.txt
    # rm results/*.png

    make all
    if [ $? -ne 0 ]; then
        echo "make all failed"
        exit 1
    fi
}


# Get system information (hostname, cpu model, cpu cores, os version, gcc version)
function get_system_info {
    {   echo "Hostname: $(hostname)"
        echo "CPU Model: $(lscpu | grep 'Model name' | awk -F: '{print $2}' | xargs)"
        echo "CPU Cores: $(nproc)"
        echo "OS Version: $(lsb_release -d | awk -F: '{print $2}' | xargs)"
        echo "GCC Version: $(gcc --version | head -n 1)"
    } > results/system_info.txt
}






THREADS=(2 4 8 16)
NUM_RUNS=3
# Function to run tests
test_gauss() {
    local size=$1
    local mode=$2
    local algorithm=$3
    local threads=$4
    local schedule=$5

    # echo "Running: size=$size, mode=$mode, algorithm=$algorithm, threads=$threads, schedule=$schedule" >> $OUTPUT_FILE
    ./bin/gauss $size $mode $algorithm $threads $schedule
}

function run_gauss_tests() {
    SIZES=(100 1000 5000 10000)
    SCHEDULES=("static" "dynamic" "guided")

    # Serial tests
    echo "Running serial tests..."
    for size in "${SIZES[@]}"; do
        for algorithm in by_line by_column; do
            for run in $(seq 1 $NUM_RUNS); do
                test_gauss $size serial $algorithm 1 default
            done
        done
    done

    # Parallel tests
    echo "Running parallel tests..."
    for size in "${SIZES[@]}"; do
        for threads in "${THREADS[@]}"; do
            for algorithm in by_line by_column; do
                for schedule in "${SCHEDULES[@]}"; do
                    for run in $(seq 1 $NUM_RUNS); do
                        test_gauss $size parallel $algorithm $threads $schedule
                    done
                done
            done
        done
    done
}


GENERATIONS=1000
test_game_of_life() {
    local rows=$1
    local cols=$2
    local mode=$3
    local threads=$4

    # echo "Running: size=$size, mode=$mode, algorithm=$algorithm, threads=$threads, schedule=$schedule" >> $OUTPUT_FILE
    ./bin/game_of_life $GENERATIONS $rows $cols $mode $threads
}

function run_game_of_life_tests(){
    SIZES=(64 1024 2048)

    # Parallel tests
    for size in "${SIZES[@]}"; do
        for threads in "${THREADS[@]}"; do
            for run in $(seq 1 $NUM_RUNS); do
                test_game_of_life $size $size 1 $threads
            done
        done
    done

    # Serial tests
    for size in "${SIZES[@]}"; do
        for run in $(seq 1 $NUM_RUNS); do
            test_game_of_life $size $size 0 1
        done
    done
}

function make_graphs {
    python3 graphs.py
}

# clean_make
# get_system_info

# run_gauss_tests
# run_game_of_life_tests

make_graphs
