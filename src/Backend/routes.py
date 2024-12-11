from flask import Blueprint, request, jsonify
from morse_utils import process_letter, handle_command

morse_bp = Blueprint('morse', __name__)

@morse_bp.route('/handle_morse/letter', methods=['POST'])
def handle_letter():
    data = request.json
    letter = data.get('letter')
    if not letter:
        return jsonify({"error": "No letter provided"}),400
    response = process_letter(letter)
    return jsonify(response)

@morse_bp.route('/handle_morse/action', methods=['POST'])
def handle_action():
    data = request.json
    action = data.get('action')
    if not action:
        return jsonify({"error": "No action provided"}),400
    response = handle_command(action)
    return jsonify(response)

@morse_bp.route('/api/recover_morse',methods=['GET'])
def recover_morse():
    data = {
        'buffer': '... --- ...',
        'sentence': 'SOS',
        'next_word': 'HELP',
    }
    return jsonify(data)