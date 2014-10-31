//////////////////////////////////////////////////////////////////////////////
// Authors:  Pooja Srivastava, Kevin Truong.
//
// Project 3: Tuning Hash Tables, Version 3.
//
//////////////////////////////////////////////////////////////////////////////
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

const string FILENAME = "pg1497.txt";

// Superclass for objects that may be stored in data structures.
class Element {
public:
};

// String element.
class StringElement : public Element {
public:
  StringElement(string _value) { set_value(_value); }

  string get_value() { return value; }
  void set_value(string _value) { value = _value; }

  void print() { cout << value << endl; }

private:
  string value;
};

// Abstract class for objects that may be stored in hash tables. Note
// the two abstract member functions that subclasses must
// implement. Also note that HashElement is a subclass of Element, so
// all HashElement objects are also Element objects. If an element can
// go in a hashtable it can certainly go in a list, stack, or queue.
class HashElement : public Element {
public:
  virtual bool equals(HashElement* other) = 0;
  virtual unsigned hash_code() = 0;
};

// Word count entry. Stores a word in a string, and an int count of
// how many times that word ocurred.
class WordCount : public HashElement {
public:
  // Default constructor; empty string and zero count.
  WordCount() {
    word = "";
    count = 0;
  }

  WordCount(string _word, int _count) {
    word = _word;
    count = _count;
  }

  // Consider WordCounts equal when their _words_ are equal. This
  // _ignores counts_.
  virtual bool equals(HashElement* other) {
    return word == static_cast<WordCount*>(other)->word;
  }

  // Hash function.
  virtual unsigned hash_code() {
    //////////////////////////////////////////////////////////////////////
    // YOU MUST MODIFY THIS FUNCTION DEFINITION IN VERSION 3
    //////////////////////////////////////////////////////////////////////

    // PROCESS - Hash Code function to store each word count efficiently
    assert(!word.empty());
    return ((53 * word.size() + word[0]) % 100052);

    //////////////////////////////////////////////////////////////////////
    // END OF CODE TO MODIFY
    //////////////////////////////////////////////////////////////////////
  }

  // Setters/getters.

  int get_count() { return count; }
  void set_count(int x) { count = x; }
  void increment_count() { count++; }

  string get_word() { return word; }
  void set_word(string x) { word = x; }

private:
  string word;
  int count;
};

////////////////////
// ArrayedList, copied from ideone.com
////////////////////

// As stated in the requirements document, 300,000 is enough capacity
// for every token.
const int ARRAYED_LIST_CAPACITY = 300000;

class ArrayedList {
public:
  ArrayedList() {
	  elements = new Element*[ARRAYED_LIST_CAPACITY];
	  used = 0;
  }

  ~ArrayedList() {
	  clear();
	  delete elements;
  }

  int size() {
	  return used;
  }

  Element* get(int i) {
    assert((i >= 0) && (i < used));
    return at(i);
  }

  void set(int i, Element* x) {
    assert((i >= 0) && (i < used));
    at(i) = x;
  }

  void insert(int i, Element* x) {
    if (used == 0) {
      assert(i == 0);
      at(0) = x;
      used = 1;
    } else {
      assert(used < ARRAYED_LIST_CAPACITY);
      for (int j = used; j > i; j--)
        at(j) = at(j-1);
     at(i) = x;
      used++;
    }
  }

  void clear() {
    for (int i = 0; i < used; i++)
      delete at(i);
    used = 0;
  }

private:
  Element*& at(int i) {
	  return elements[i];
  }

  Element **elements;
  int used;
};

////////////////////
// Hashtable
////////////////////

struct HashNode {
  HashNode(HashElement* contents_, HashNode* next_) {
    contents = contents_;
    next = next_;
  }

  HashElement* contents;
  HashNode* next;
};

class Hashtable {
public:
  // See Open Data Structures section 5.1 for an explanation of d and
  // z.
  Hashtable(int expected_size) {
    n = 0;
    d = static_cast<int>(log2(expected_size * 2));
    z = rand();

    buckets = new HashNode*[capacity()];
    for (int i = 0; i < capacity(); i++)
      buckets[i] = NULL;
  }

  ~Hashtable() { clear(); }

  int size() { return n; }
  bool is_empty() { return n == 0; }

  HashElement* find(HashElement* x) {
    HashNode* p = find_node(x);
    if (p == NULL)
      return NULL;
    else
      return p->contents;
  }

  bool add(HashElement* x) {
    HashNode* p = find_node(x);
    if (p == NULL) {
      unsigned i = hash(x);
      buckets[i] = new HashNode(x, buckets[i]);
      n++;
      return true;
    } else {
      p->contents = x;
      return true;
    }
  }

  void clear() {
    for (int i = 0; i < capacity(); i++) {
      HashNode* p = buckets[i];
      while (p != NULL) {
	HashNode* next = p->next;
	delete p;
	p = next;
      }
      buckets[i] = NULL;
    }
    n = 0;
  }

private:
  // The ceiling of the base-2 logarithm of x.
  int log2(int x) {
    return static_cast<int>(ceil(log(x) / log(2)));
  }

  // 2 raised to the power x.
  int pow2(int x) {
    return 1 << x;
  }

  // Capacity of the hash table array.
  int capacity() {
    return pow2(d);
  }

  // Compute a hash value (array index) for x.
  unsigned hash(HashElement* x) {
    assert(x != NULL);
    // Multiplicative hashing:
    unsigned i = (z * x->hash_code()) >> (32 - d);
    assert(i >= 0);
    assert(i < static_cast<unsigned>(capacity()));
    return i;
  }

  HashNode* find_node(HashElement* x) {
    HashNode* p = buckets[hash(x)];
    while (p != NULL) {
      if (p->contents->equals(x))
	return p;
      p = p->next;
    }
    return NULL;
  }

  int n, z, d;
  HashNode** buckets;
};

int main() {
  // Load each token in the text file into an ArrayedList. You may
  // reuse this code.
  cout << "Loading words... ";
  ArrayedList tokens;
  ifstream f(FILENAME.c_str()); // open file
  if (!f)
  {
	  // make sure that worked
	  cout << "ERROR: cannot open " << FILENAME << endl;
	  return 1;
  }

  // Read each token
  while (f)
  {
    string token;
    f >> token;

    if (!token.empty())
      tokens.insert(tokens.size(), new StringElement(token));
  }
  // close file
  f.close();

  cout << tokens.size() << " tokens in document" << endl;
  cout << "Counting word frequencies..." << endl;

  //PROCESS - Hashtable used to store the number of word counts
  Hashtable wordCounts(tokens.size());

  //PROCESS - Used to calculate time
  clock_t start, stop, elapsed_ticks;
  double elapsed_seconds;

  //PROCESS - Used to check for matches if not creates a new word count
  WordCount *match = new WordCount("", 1);

  //PROCESS - Gives us access to the word and its count
  WordCount *p;
  int counter = 0;

  start = clock();

  //PROCESS - Go through each token to see if it's in WordCounts
  for(int i = 0; i < tokens.size(); i++)
  {
	  //PROCESS - We grab the string from the token and set it to match
	  string token;
	  token = static_cast<StringElement*>(tokens.get(i))->get_value();
	  match->set_word(token);

	  p = static_cast<WordCount*>(wordCounts.find(match));

	  //PROCESS - If the word exists we increase its count
	  if (p != NULL)
	  {
		  p->increment_count();
	  }
	  //PROCESS - If the does not already exist we add it to our table and
	  //		  set its count = to 1.
	  else
	  {
		  wordCounts.add(new WordCount(token, 1));
		  counter++;
	  }
  }

  //OUTPUT - We output the single count of words that appear in the text
  cout << "Number of distinct words: " << counter << endl << endl;

  //PROCESS/OUTPUT - If the word matches the set word we output its count
  match->set_word("the");
  p = static_cast<WordCount*>(wordCounts.find(match));
  cout << "Occurrences of 'the': " << p->get_count() << endl;

  match->set_word("of");
  p = static_cast<WordCount*>(wordCounts.find(match));
  cout << "Occurrences of 'of': " << p->get_count() << endl;

  match->set_word("and");
  p = static_cast<WordCount*>(wordCounts.find(match));
  cout << "Occurrences of 'and': " << p->get_count() << endl;

  match->set_word("nature");
  p = static_cast<WordCount*>(wordCounts.find(match));
  cout << "Occurrences of 'nature': " << p->get_count() << endl;

  //PROCESS - We stop the timer
  stop = clock();

  //PROCESS - Compute how many "clock" units of time elapsed
  elapsed_ticks = stop - start;

  //PROCESS - Convert to seconds
  elapsed_seconds = elapsed_ticks / static_cast<double>(CLOCKS_PER_SEC);

  //OUTPUT - The total time
  cout << "Elapsed time = " << elapsed_seconds << " sec." << endl;

  return 0;
}

