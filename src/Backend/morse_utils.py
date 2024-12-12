import time
import torch
from transformers import GPT2Tokenizer, GPT2LMHeadModel
from firebase_admin import firestore
from config import MORSE_CODE
from firebase_methods import save_to_firestore

tokenizer = GPT2Tokenizer.from_pretrained('gpt2')
model = GPT2LMHeadModel.from_pretrained('gpt2')

letter_buffer = ""
sentence_buffer  =""
time_differences = []
last_letter_time = None
next_word = ""

def predict_next_word(context):
    context = context[-50:]
    tokenizer.pad_token = tokenizer.eos_token
    inputs = tokenizer(context, return_tensors='pt', truncation=True, padding=True)

    with torch.no_grad():
        outputs = model.generate(
            inputs['input_ids'],
            attention_mask = inputs['attention_mask'],
            max_length = inputs['input_ids'].shape[1] + 5,
            num_return_sequences = 1,
            no_repeat_ngram_size = 2,
            pad_token_id = tokenizer.pad_token_id
        )
        predict_text = tokenizer.decode(outputs[0], skip_special_tokens=True)
        predict_tokens = predict_text[len(context):].strip().split()
        return predict_tokens[0] if predict_tokens else ""

def process_letter(letter):
    global letter_buffer, sentence_buffer, last_letter_time, time_differences, next_word
    current_time = time.time()
    if last_letter_time:
        time_differences.append(current_time-last_letter_time)
    last_letter_time = current_time

    letter_buffer += letter
    if letter == ' ' or letter.endswith('.'):
        word_length = len(letter_buffer.strip())
        pulse_types = [MORSE_CODE.get(char.upper(), '?') for char in letter_buffer]
        short_total = sum(pulse.count('.') for pulse in pulse_types)
        long_total = sum(pulse.count('-') for pulse in pulse_types)

        data = {
            'word': letter_buffer.strip(),
            'word_length': word_length,
            'time_between_data': time_differences,
            'shorts': short_total,
            'longs': long_total,
            'timeStamp': firestore.SERVER_TIMESTAMP
        }
        save_to_firestore(data)
        sentence_buffer += letter_buffer.strip() + ' '
        letter_buffer = ""
        time_differences = []
        word_length = 0
        next_word = predict_next_word(sentence_buffer)
        return {"next_word": next_word, "sentence": sentence_buffer}
    return {"letter_buffer": letter_buffer}

def handle_command(action):
    if action == 'Accept':
        global next_word, sentence_buffer
        sentence_buffer += next_word + ' '
        return {"message": "Command accept"}
    elif action == 'Erase':
        global letter_buffer
        letter_buffer = letter_buffer[:-1]
        return {"message": "Last letter erased"}
    else:
        return {"error": "unknown command"}

def get_NextWord():
    global next_word
    return next_word if next_word else "Making prediction"

def get_sentence():
    global sentence_buffer
    return sentence_buffer if sentence_buffer else "Write something"

def get_word():
    global letter_buffer
    return letter_buffer if letter_buffer else "Write something"