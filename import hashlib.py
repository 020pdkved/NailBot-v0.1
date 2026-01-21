import hashlib
text= input ("type you plain text here: ")
print(hashlib.blake2b(text.encode()).hexdigest())