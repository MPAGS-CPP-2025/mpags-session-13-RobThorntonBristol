#include "PlayfairCipher.hpp"

#include <algorithm>
#include <string>
#include <iostream>

/**
 * \file PlayfairCipher.cpp
 * \brief Contains the implementation of the PlayfairCipher class
 */

PlayfairCipher::PlayfairCipher(const std::string& key)
{
    this->setKey(key);
}

void PlayfairCipher::setKey(const std::string& key)
{
    // Store the original key
    key_ = key;

    // Append the alphabet to the key
    key_ += "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    // Make sure the key is upper case
    std::transform(std::begin(key_), std::end(key_), std::begin(key_),
                   ::toupper);

    // Remove non-alphabet characters
    key_.erase(std::remove_if(std::begin(key_), std::end(key_),
                              [](char c) { return !std::isalpha(c); }),
               std::end(key_));

    // Change J -> I
    std::transform(std::begin(key_), std::end(key_), std::begin(key_),
                   [](char c) { return (c == 'J') ? 'I' : c; });

    // Remove duplicated letters
    std::string lettersFound{""};
    auto detectDuplicates = [&](char c) {
        if (lettersFound.find(c) == std::string::npos) {
            lettersFound += c;
            return false;
        } else {
            return true;
        }
    };
    key_.erase(
        std::remove_if(std::begin(key_), std::end(key_), detectDuplicates),
        std::end(key_));

    // Store the coordinates of each letter
    // (at this point the key length must be equal to the square of the grid dimension)
    for (std::size_t i{0}; i < keyLength_; ++i) {
        std::size_t row{i / gridSize_};
        std::size_t column{i % gridSize_};

        auto coords = std::make_pair(row, column);

        charLookup_[key_[i]] = coords;
        coordLookup_[coords] = key_[i];
    }
}

std::string PlayfairCipher::applyCipher(const std::string& inputText,
                                        const CipherMode /*cipherMode*/) const
{
    // Create the output string, initially a copy of the input text
    std::string outputText{inputText};

    // Change J -> I
    std::transform(std::begin(outputText), std::end(outputText), std::begin(outputText),
                   [](char c) { return (c == 'J') ? 'I' : c; });

    // Find repeated characters and add an X (or a Q for repeated X's)
    // If the size of the input is odd, add a trailing Z
    std::string result{};

    for (size_t i = 0; i < outputText.length(); ) {
        char first = outputText[i];
        char second{};

        if (i + 1 < outputText.length())
            second = outputText[i + 1];
        else
            second = (first == 'Z') ? 'X' : 'Z';

        if (first == second && first != 'X') {
            result += first;
            result += 'X';
            i += 1;
        } else if (first == second && first == 'X') {
            result += first;
            result += 'Q';
            i += 1;
        } else {
            result += first;
            result += second;
            i += 2;
        }
    }

    outputText = result;
    
    std::cout << outputText << std::endl;


    std::string encrypted{};

    // Loop over the input bigrams
    for (size_t i = 0; i < outputText.length(); i += 2) {
        char a = outputText[i];
        char b = outputText[i + 1];

        // - Find the coordinates in the grid for each bigram
        auto [r1, c1] = charLookup_.at(a);
        auto [r2, c2] = charLookup_.at(b);

        // - Apply the rules to these coords to get new coords
        if (r1 == r2) {
            // Same row → shift right
            c1 = (c1 + 1) % gridSize_;
            c2 = (c2 + 1) % gridSize_;
        } 
        else if (c1 == c2) {
            // Same column → shift down
            r1 = (r1 + 1) % gridSize_;
            r2 = (r2 + 1) % gridSize_;
        } 
        else {
            // Rectangle → swap columns
            std::swap(c1, c2);
        }

        // - Find the letters associated with the new coords
        encrypted += coordLookup_.at({r1, c1});
        encrypted += coordLookup_.at({r2, c2});
    }

    outputText = encrypted;

    // Return the output text
    return outputText;
}
