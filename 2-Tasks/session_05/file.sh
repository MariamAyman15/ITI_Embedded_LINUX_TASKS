#!/bin/bash

# Define the phone book file
phone_book_file="phone_book.txt"

# Check if the phone book file exists, if not create it
if [ ! -f "$phone_book_file" ]; then
  touch "$phone_book_file"
fi

# Define the functions to add, edit, remove and search contacts
function add_contact {
  echo "Enter contact name:"
  read name
  echo "Enter contact phone number:"
  read number
  echo "$name:$number" >> "$phone_book_file"
  echo "Contact added successfully."
}

function edit_contact {
  echo "Enter contact name to edit:"
  read search_name
  if grep -qi "$search_name" "$phone_book_file"; then
    echo "Enter new phone number:"
    read new_number
    sed -i "s/^$search_name:.*/$search_name:$new_number/" "$phone_book_file"
    echo "Contact updated successfully."
  else
    echo "Contact not found."
  fi
}

function remove_contact {
  echo "Enter contact name to remove:"
  read search_name
  if grep -qi "$search_name" "$phone_book_file"; then
    sed -i "/^$search_name:.*/d" "$phone_book_file"
    echo "Contact removed successfully."
  else
    echo "Contact not found."
  fi
}

function search_contact {
  echo "Enter contact name to search:"
  read search_name
  grep -i "$search_name" "$phone_book_file"
}

# Define the main menu
function main_menu {
  echo "Phone Book"
  echo "1. Add contact"
  echo "2. Edit contact"
  echo "3. Remove contact"
  echo "4. Search contact"
  echo "5. Exit"

  read choice
  case $choice in
    1)
      add_contact
      ;;
    2)
      edit_contact
      ;;
    3)
      remove_contact
      ;;
    4)
      search_contact
      ;;
    5)
      exit 0
      ;;
    *)
      echo "Invalid choice"
      ;;
  esac
}

# Run the main menu in a loop
while true; do
  main_menu
done