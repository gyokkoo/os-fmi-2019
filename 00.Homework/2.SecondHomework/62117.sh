#!/bin/bash

# Operating Systems second homework script
# See 62117.txt for documentation
# Author: Gyokan Syuleymanov

# Function to output useful information
usage() {
    cat <<END
How to use this script?
    \$1: Log file with 3 columns: <stimul> <stimul_start> <stimul_end>
    \$2: EEG file in xml format that contains StartRecordingDate, StartRecordingTime, SaplingRate and ticks
    \$3: Standard wav file from laryngograph
    \$4: Destination directory
The script uses EEG_TZ variable if not set use UTC
Sample script execution:
    $ export EEG_TZ='Europe/Sofia'
    $ ./62117.sh timetable.log eeg.xml larynx.wav ~/output_dir 
END
}

# Function to handle errors
# First argument: error message to print
# Second argument: exti code to exit script with
error() {
    echo "Error: $1"
    usage
    exit $2
} >&2

ignore_period_limit=0.02
log_file="$1"
eeg_file="$2"
wav_file="$3"
output_dir="$4"

echo "Validating input..."
if [[ ! -r $log_file ]]; then
    error "Missing readable log file argument" 1
elif [[ $(awk '{print NF}' $log_file | sort -nu) -ne "3" ]]; then
    error "Each line of $log_file must contain three columns" 1
elif [[ $(cat $log_file | cut -d ' ' -f 1 | sort | uniq -d | wc -l) -gt 0 ]]; then
    error "Duplicate stimules found" 1
elif [[ $(grep -c 'beep' $log_file) -eq 0 ]]; then
    error "Beep stimul not found!" 1
fi

if [[ ! -r $eeg_file ]]; then
    error "Missing readable eeg file argument" 1
elif [[ $(cat $eeg_file | head -n 100 | grep -c 'StartRecordingDate') -eq 0 ]]; then
    error "StartRecordingDate is missing from $eeg_file" 1
elif [[ $(cat $eeg_file | head -n 100 | grep -c 'StartRecordingTime') -eq 0 ]]; then
    error "StartRecordingTime is missing from $eeg_file" 1
elif [[ $(cat $eeg_file | head -n 100 | grep -c 'SamplingRate') -eq 0 ]]; then
    error "SamplingRate is missing from $eeg_file" 1
fi

if [[ ! -r $wav_file ]]; then
    error "Missing wav file argument" 1
fi

if [[ ! -d $output_dir ]]; then
    # If not: create output directory
    if mkdir $output_dir; then
        echo "Created output directory ${output_dir}"
    else
        error "Could not create ${outpu_dir}" 1
    fi
fi
echo "Validation passed!"

echo "Started execution of $0 with arguments: $@ ..."

echo "Extracting values from ${eeg_file}..."
xml_header_data=$(cat ${eeg_file} | head -n 50)
start_recording_time=$(grep -oPm1 "(?<=<StartRecordingTime>)[^<]+" <<< "$xml_header_data")
start_recording_date=$(grep -oPm1 "(?<=<StartRecordingDate>)[^<]+" <<< "$xml_header_data")
sampling_rate=$(grep -oPm1 "(?<=<SamplingRate>)[^<]+" <<< "$xml_header_data" | cut -d ' ' -f 1)

echo "StartRecodingTime=$start_recording_time"
echo "StartRecorsingDate=$start_recording_date"
echo "SamplingRate=$sampling_rate"

if [[ -z $EEG_TZ ]]; then
    echo "Warning! Variable EEG_TZ is not set, using UTC time zone"
    EEG_TZ="UTC"
else
    echo "EEG_TZ=${EEG_TZ}"
fi

start_date_formatted=$(echo $start_recording_date | awk -F '.' -v OFS='-'  '{print $3, $2, $1}')
eeg_start_time="$start_date_formatted $start_recording_time"
eeg_start_timestamp="$(TZ="$EEG_TZ" date -d "$eeg_start_time" "+%s" )"
eeg_first_tick_line_number="$(grep -n '<tick>' $eeg_file | head -n 1 | cut -d ':' -f 1 )"

wav_start_time=""

echo "Reading ${log_file}..."
while read -r log_line; do
    echo "____________________________________________________________________"
    echo "Parsing stimul ${log_line}"
    stimul_name=$(echo $log_line | cut -d ' ' -f 1) 
    stimul_start_time=$(echo $log_line | cut -d ' ' -f 2)
    stimul_end_time=$(echo $log_line | cut -d ' ' -f 3)

    if (( $(echo "$stimul_start_time > $stimul_end_time" | bc) )); then
        error "Invalid stimul period, end time is before start time!" 1
    fi

    duration=$(echo "$stimul_end_time - $stimul_start_time" | bc)
    if (( $(echo "$duration < $ignore_period_limit" | bc) )); then
        echo "Warning! Ignoring stimul ${stimul_name}, it has less than ${ignore_period_limit} seconds duration.\n\n\n"
        continue;
    fi

    if [[ $has_audio_file_started == "true" ]]; then
        eeg_delta_seconds=$(echo "$stimul_start_time - $eeg_start_timestamp" | bc)
        stimul_tick_start=$(echo "$eeg_delta_seconds * $sampling_rate + $eeg_first_tick_line_number" | bc)
        stimul_tick_end=$(echo "$stimul_tick_start + $duration * $sampling_rate" | bc)
        # Round to whole numbers 
        tick_start_number=$(echo "$stimul_tick_start/1" | bc)
        tick_end_number=$(echo "$stimul_tick_end/1" | bc)

        echo "Cutting ${tick_start_number} - ${tick_end_number} ticks from ${eeg_file} "
        sed -n "${tick_start_number},${tick_end_number} p" "${eeg_file}" > "${output_dir}/${stimul_name}_eeg.xml"
        echo "Saved successfully ${stimul_name}_eeg.xml"

        wav_start_seconds=$(echo "$stimul_start_time - $wav_start_time" | bc)
        wav_end_seconds=$(echo "$wav_start_seconds + $duration" | bc)
        echo "Cutting ${wav_start_seconds} - ${wav_end_seconds} seconds from wav"
        sox "${wav_file}" "${output_dir}/${stimul_name}_lar.wav" trim $wav_start_seconds $duration
        echo "Saved successfully ${stimul_name}_lar.wav"
    fi

    if [[ $stimul_name == 'beep' ]]; then
        echo "Beep stimul found!"
        eeg_passed_seconds=$(echo "($stimul_end_time - $eeg_start_timestamp)" | bc -l)
    	echo "Seconds after recording start: $eeg_passed_seconds"
        if (( $(echo "$eeg_passed_seconds <= 0" | bc) )); then
            error "EEG file has not started yet when parsing stimuls, check the time!" 1
        fi
        wav_start_time=$stimul_end_time
        has_audio_file_started="true"
    fi
done < "$log_file"

echo "Script execution finished successfully! Check ${output_dir} for command results"
exit 0
