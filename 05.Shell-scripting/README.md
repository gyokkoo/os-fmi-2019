## Shell скриптове

#### Pull requests with improvements or solutions are more than welcome :)

**Зад 1. 05-а-2000**<br/>Сменете вашия prompt с нещо по желание. После върнете оригиналния обратно.

`PS_OLD=$PS1`

`$PS1="enter command"`

`PS1=$PS_Old`

**Зад 2. 05-а-2100**<br/>Редактирайте вашия .bash_profile файл, за да ви поздравява (или да изпълнява някаква команда по ваш избор) всеки път, когато влезете в системата.

`vi ~/.bash_profile`  insert `echo "Welcome to unix, dear friend!"`

**Зад 3. 05-а-2200**<br/>Направете си ваш псевдоним (alias) на полезна команда.

`vi ~/bashrc` insert `alias c='clear'`

**Зад 4. 05-а-4000**<br/>Напишете shell script (който ще използвате от вашия линукс), който да приема параметър име на файл, който искате да копирате от os-server и да го копира в текущата директория (на вашия линукс).

Пример за употреба:

```
# сваля /etc/passwd от os-server на вашия линукс
./get_remote_file /etc/passwd
```

`vi ~/bin/get_remote_file`

```bash
#!/bin/bash

# get_remote_file
# This script copies a file from remote unix to local unix machine

# Check for an argument
if [[ ! $1  ]]; then
    echo "Missing filename argument"
    exit 1
fi

filename="$1"
destination="$PWD"

echo "Trying to get ${filename} from remote server"

scp "s62117@os-server:${filename}" "${destination}"
```

`chmod +x ~/bin/get_remote_file`

**Зад 5. 05-b-2000**<br/>Да се напише shell скрипт, който приканва потребителя да въведе име (низ) и изпечатва "Hello, името".

````bash
#!/bin/bash

# Ask for user input
read -p "What is your name?": user_input

echo "Hello, $user_input"
````

**Зад 6. 05-b-2800**<br/>Да се напише shell скрипт, който проверява дали подаденият му параметър се състои само от букви и цифри.

````bash
#!/bin/bash
if [[ ! $1 ]]; then
    echo "Missing argument"
    exit 1
fi

if [[ ! $1 =~ [^a-zA-Z0-9]+ ]]; then
    echo "Your input contains only letters and digits!"
else
    echo "Your input does not contain only letter and digits!"
fi
````

**Зад 7. 05-b-3000**<br/>Да се напише shell скрипт, който извежда на стандартния изход факултетния номер на потребителя, който изпълнява скрипта, или казва "DANGER! It's a teacher!", ако потребителското име не е факултетен номер.

``````bash
#!/bin/bash
username=$(whoami)
if [[ $username =~ ^s[0-9]+$ ]]; then
    echo "${username:1}" # Cut first symbol
else
    echo "DANGER! It's a teacher!"
fi
``````

**Зад 8. 05-b-3100**<br/>Да се напише shell скрипт, който приканва потребителя да въведе низ - потребителско име на потребител от системата - след което извежда на стандартния изход колко пъти потребителят се е логнал.

``````bash
#!/bin/bash

# Task 05-b-3200
read -p "Enter username from the system:" username

# Check if user exists in the system
users_count=$(cat /etc/passwd | cut -d ':' -f 1 | egrep -c "^$username\$")
if [[ $users_count -ne 1 ]]; then
    echo "User ${username} does not exist in the system"
    exit 1
fi

# command who contains information for logged in users in the system
user_logged_count=$(echo "$(who)" | awk '{print $1}' | egrep -c "^$username\$")
if [[ $user_logged_count -eq 0 ]]; then
    echo "User ${username} is not logged in -> 0"
else
    echo "User ${userame} is logged in ${user_logged_count} times"
fi

exit 0
``````

**Зад 9. 05-b-3200**<br/>Да се напише shell скрипт, който приканва потребителя да въведе пълното име на директория и извежда на стандартния изход подходящо съобщение за броя на всички файлове и директории в нея.

````bash
#!/bin/bash
read -p "Enter directory: " dir

if [[ ! -d $dir ]]; then
    echo "${dir} is not a directory"
    exit 1
fi

files_count=$(find "$dir" -type f | wc -l)
directories_count=$(find "$dir" -type d | wc -l)
echo "${dir} has total ${files_count} files and ${directories_count} directories"
exit 0
````

**Зад 10. 05-b-3300**<br/>Да се напише shell скрипт, който чете от стандартния вход имената на 3 файла, обединява редовете на първите два, подрежда ги по азбучен ред и резултата записва в третия файл.

```  bash
#!/bin/bash
read -p "Enter first file: " firstFile
read -p "Enter second file: " secondFile
read -p "Enter output file: " output_file

echo"Reading the content of first two files and saving it sorted in the third file..."
cat $firstFile $secondFile | sort > $output_file
echo "Result saved in ${output_file}"
```

**Зад 11. 05-b-3400**<br/>Да се напише shell скрипт, който чете от стандартния вход име на файл и символен низ, проверява дали низа се съдържа във файла и извежда на стандартния изход кода на завършване на командата.

```bash
#!/bin/bash

read -p "Enter text file: " input_file
read -p "String to search: " target_string

echo "Searching \"${target_string}\" in ${input_file}"
grep_exit_code=$(grep -q "${target_string}" ${input_file} ; echo $?)

echo "$grep_exit_code"
```

**Зад 12. 05-b-4200** <br/>Имате компилируем (a.k.a няма синтактични грешки) source file на езика C. Напишете shell script, който да покзва колко е дълбоко най-дълбокото nest-ване (влагане). По-лесно ли щеше да е за файл с Python код?

Примерен .c файл:

```c
int main(int argc, char *argv[]) {
	
	if (argc == 1) {
		printf("There is only 1 argument");
	} else {
		printf("There are more than 1 arguments");
	}
	
	return 0;
}
```

Тук влагането е 2, понеже имаме main блок, а вътре в него if блок.

Примерно извикване на скрипта: <br/> ./count_nesting sum_c_code.c

Изход: <br/>The deepest nesting is 2 levels

`````bash
#!/bin/bash

c_file=$1
if [[ ! -r ${c_file} ]]; then
    echo "Error! File ${c_file} is not readable!"
    exit 1
fi
if [ ${file:-2} == ".c" ]; then
    echo "Error! File ${c_file} has not .c extension!"
    exit 1
fi

# -i is stands for integer
declare -i max_nesting=0
declare -i nesting=0

# Read input symbol by symbol
while IFS= read -r -n 1 symbol; do
    if [[ $symbol == "}" ]]; then
        nesting=$((nesting-1))
    elif [[ $symbol == "{" ]]; then
        nesting=$((nesting+1))
    fi

    if (( max_nesting < nesting )); then
        max_nesting=$nesting
    fi
done < "$c_file"

echo  "The deepest nesting is ${max_nesting} levels"
`````

**Зад 13. 05-b-4300**<br/>Напишете shell script, който по подаден като аргумент файл с map между \<nickname\> -> <реален sername в os-server> и nickname, ще ви улесни да изпращате съобщения на хората.

Пример за файл указател: 

```
tinko	s61966
minko	s881234
ginko	s62000
dinko	s77777
senpai	velin
```

Примерно извикване на програмата:<br/>./send_message myAddressBook dinko

`````bash
#!/bin/bash

address_book=$1
target=$2

if [[ ! -r $address_book ]]; then
    echo "Error! ${address_book} is not readable"
    exit 1
fi

if [[ $(awk '{print NF}' $address_book | sort -nu) -ne "2" ]]; then
    echo "Address book must have exactly 2 columns"
    exit 1
fi

target_username=$(cat "${address_book}" | egrep "$target " | cut -d ' ' -f 2)
if [[ -z $target_username ]]; then
    echo "Username with nickname ${target} cannot be found in ${address_book}"
    exit 1
fi

read -p "Enter message: " message
echo "Sending message to ${target_username} "
write ${target_username} "${message}"
exit # Exit with exit code from last command
`````

**Зад 14. 05-b-4301** <br/>Напишете shell script, който автоматично да попълва файла указател от предната задача по подадени аргументи: име на файла указател, пълно име на човека (това, което очакваме да е в /etc/passwd) и избран за него nickname.<br/>Файлът указател нека да е във формат:<br/><nickname, който лесно да запомните> <username в os-server>

Примерно извикване:<br/>./pupulate_address_book myAddressBook "Ben Dover" uncleBen

Добавя към myAddressBook entry-то:<br/>uncleBen <username на Ben Dover в os-server>

*Бонус: Ако има няколко съвпадения за въведеното име (напр. има 10 човека Ivan Petrov в /etc/passwd), всички те да се показват на потребителя, заедно с пореден номер >=1,след което той да може да въведе някой от номерата (или 0 ако не си хареса никого), и само избраният да бъде добавен към указателя.*

``````
// TODO
``````

**Зад 15. 05-b-4400**<br/>Напишете shell script, който да приема параметър име на директория, от която взимаме файлове, и опционално експлицитно име на директория, в която ще копираме файлове. Скриптът да копира файловете със съдържание, променено преди по-малко от 45 мин, от първата директория във втората директория. Ако втората директория не е подадена по име, нека да получи такова от днешната дата във формат, който ви е удобен. При желание новосъздадената директория да се архивира.

``````bash
#!/bin/bash

set -x
src_dir=$1
if [[ ! -d $src_dir ]]; then
    echo "Error! ${src_dir} is not a directory!"
    exit 1
fi

dst_dir=$2
if [[ -z $dst_dir ]]; then
    dst_dir="${HOME}/$(date | tr -s ' ' '_')"
    mkdir ${dst_dir}
    echo "Destination directory is not set, using ${dst_dir}"
fi

echo "Copying recent modified files from ${src_dir} to ${dst_dir}"
find "$src_dir" -type f -mmin -45 -exec cp '{}' ${dst_dir}  \;
echo "Done, check ${src_dir} for new files"

while true; do
    read -p "Do you want to archieve the new folder? yes / no " answer
    case $answer in
        yes)
            echo "Creating archieve"
            tar -zcvf "${dst_dir}.tar.gz" "${dst_dir}"
            echo "Archieve ${dst_dir}.tar.gz created"
            break
            ;;
        no)
            echo "Ok, bye!"
            break
            ;;
    esac
done
``````

**Зад 16. 05-b-4500**<br/>Да се напише shell скрипт, който получава при стартиране като параметър в командния ред идентификатор на потребител. Скриптът периодично (sleep(1)) да проверява дали потребителят е log-нат, и ако да - да прекратява изпълнението си, извеждайки на стандартния изход подходящо съобщение.

``````
// TODO
``````

**Зад 17. 05-b-4600**<br/>Да се напише shell скрипт, който валидира дали дадено цяло число попада в целочислен интервал. Скриптът приема 3 аргумента: числото, което трябва да се провери; лява граница на интервала; дясна граница на интервала.

Скриптът да връща exit status:

- 3, когато поне един от трите аргумента не е цяло число

- 2, когато границите на интервала са обърнати

- 1, когато числото не попада в интервала

- 0, когато числото попада в интервала

  ```
  Примери:
  $ ./validint.sh -42 0 102; echo $?
  1
  
  $ ./validint.sh 88 94 280; echo $?
  1
  
  $ ./validint.sh 32 42 0; echo $?
  2
  
  $ ./validint.sh asdf - 280; echo $?
  3
  ```

``````bash
#!/bin/bash

target=$1
left=$2
right=$3

num_regex="^[+-]?[0-9]+([.][0-9]+)?$"
if [[ ! $target =~ $num_regex || ! $left =~ $num_regex || ! $right =~ $num_regex ]]; then
    exit 3
fi

if (( $left <= $target )) && (( $target <= $right )); then
    exit 0
elif (( $left >= $target )) && (( $target >= $right )); then
    exit 2
fi

if (( $left > $target )) || (($target > $right )); then
    exit 1
fi
``````

**Зад 18. 05-b-4700**<br/>Да се напише shell скрипт, който форматира големи числа, за да са по-лесни за четене. <br/>Като пръв аргумент на скрипта се подава цяло число.<br/>Като втори незадължителен аргумент се подава разделител. По подразбиране цифрите се разделят с празен интервал.

Примери:

`````
$ ./nicenumber.sh 1889734853
1 889 734 853

$ ./nicenumber.sh 7632223 ,
7,632,223
`````

``````bash
#!/bin/bash

number=$1
if [[ ! $number =~ ^[0-9]+$ ]]; then
    echo "The first argument is not a number!"
    exit 1
fi

delimeter=$2
if [[ -z $delimeter ]]; then
    delimeter=" "
    echo "Using space as default delimeter"
fi

reversed_num=$(echo "$number" | rev)
echo $(echo $reversed_num | sed "s/.\{3\}/&$delimeter/g" | rev)
``````

**Зад 19. 05-b-4800**<br/>

Да се напише shell скрипт, който приема файл и директория. Скриптът проверява в подадената директория и нейните под-директории дали съществува копие на подадения файл и отпечатва имената на намерените копия, ако съществуват такива.

``````bash
#!/bin/bash

search_for=$1
search_in=$2

if [[ ! -f $search_for ]]; then
    echo "Error! First argument is not a file!"
    exit 1
fi

if [[ ! -d $search_in ]]; then
    echo "Error! Second argument is not a directory!"
    exit 1
fi

echo "$search_in"

find "$search_in" | while read file; do
    if [ ! -f $flie ]; then
        continue;
    fi

    chk1=`cksum ${search_for} | awk -F " " '{print $1}'`
    chk2=`cksum ${file} | awk -F " " '{print $1}'`
    if [ $chk1 -eq $chk2 ]; then
        echo "$file"
    fi
done
``````

**Зад 20. 05-b-5500**<br/>

Да се напише shell script, който генерира HTML таблица със студентите в os-server. Таблицата трябва да има:

- колони за факултетен номер, име, фамилия, специалност, курс и група

- заглавен ред с имената нa колоните
  Пример:

  ```
  $ ./passwd-to-html.sh > table.html
  $ cat table.html
  <table>
    <tr>
      <th>FN</th>
      <th>Firstname</th>
      <th>Lastname</th>
      <th>Specialty</th>
      <th>Course</th>
      <th>Group</th>
    </tr>
    <tr>
      <td>60309</td>
      <td>Dragan</td>
      <td>Petkanov</td>
      <td>SI</td>
      <td>4</td>
      <td>3</td>
    </tr>
    <tr>
      <td>81111</td>
      <td>Pencho</td>
      <td>Ivanov</td>
      <td>KN</td>
      <td>3</td>
      <td>8</td>
    </tr>
  </table>
  ```

``````bash
#!/bin/bash

echo -e "<table>\n<tr>"
echo -e "\t<th>FN</th>"
echo -e "\t<th>Firstname</th>"
echo -e "\t<th>Lastname</th>"
echo -e "\t<th>Specialty</th>"
echo -e "\t<th>Course</th>"
echo -e "\t<th>Group</th>\n</tr>"

cat /etc/passwd | while read line; do
    username=$(echo $line | cut -d ':' -f 1)
    info=$(echo $line | cut -d ':' -f 5)
    fn=${username:1}

    if [[ ! $fn =~ ^[0-9]+$ ]]; then
        continue
    fi

    # TODO: Trivial, cut info into Firstname, Lastname, Specialty, Cource and Group
    echo "<tr>"
    echo -e "\t<td>$fn</td>"
    echo -e "\t<td>$info</td>"
    echo "</tr>"
done

echo "</table>"
``````

**Зад 21. 05-b-6600**<br/>

**Зад 22. 05-b-7000**<br/>

**Зад 23. 05-b-7100**<br/>

**Зад 24. 05-b-7200**<br/>

**Зад 25. 05-b-7500**<br/>Напишете shell script guess, която си намисля число, което вие трябва да познате. В зависимост от вашия отговор, програмата трябва да ви казва "надолу" или "нагоре", докато не познате числото. Когато го познаете, програмата да ви казва с колко опита сте успели.

``````bash
#!/bin/bash

# A simple guessing game

# Get a random between [1:100]
declare -ir target=$(( ($RANDOM % 100) + 1 ))

# Initialize the user's guess
declare -i guess=0
declare -i tries=0
until (( guess == target )); do
    read -p "Guess? " guess

    # If guess is 0, it was not a number
    (( guess )) || continiue

    tries=$(expr $tries + 1)
    if (( guess < target )); then
        echo "...biger!!"
    elif (( guess > target )); then
        echo "...smaller!"
    else
        echo "RIGHT! Guessed ${target} in ${tries} tries!"
    fi
done
``````



**Зад 26. 05-b-7550**<br/>

**Зад 27. 05-b-7700**<br/>

**Зад 28. 05-b-7800**<br/>

**Зад 29. 05-b-8000**<br/>

**Зад 30. 05-b-9100**<br/>

**Зад 31. 05-b-9200**<br/>

**Зад 32. 05-b-9500**<br/>

**Зад 33. 05-b-9600**<br/>

**Зад 32. 05-b-9601**<br/>





