from firebase_admin import firestore
import time
from config import db

def save_to_firestore(data):
    try:
        collection_name = "morse_data"
        document_name = f"entry_{int(time.time())}"
        db.collection(collection_name).document(document_name).set(data)
    except Exception as e:
        print(f"Error saving to firestore: {e}")

def recover_firestore():
    try:
        collection_name = "morse_data"
        docs = db.collection(collection_name).stream()
        morse_data = [{"id": doc.id, **doc.to_dict()} for doc in docs]
        return morse_data
    except Exception as e:
        print(f"Error retrieving data from Firestore: {e}")
        return []