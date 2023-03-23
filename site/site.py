from flask import Flask, render_template, request, jsonify
from flask import send_from_directory
import requests
import json

from nltk.tokenize import word_tokenize, sent_tokenize
from nltk.corpus import stopwords
from nltk.stem import PorterStemmer, WordNetLemmatizer
import string

app = Flask(__name__)

@app.route('/')
def home():
    return render_template('index.html')

stop_words = set(stopwords.words("english"))

def normalize_text(text):
    lemmatizer = WordNetLemmatizer()
    words = word_tokenize(text)
    
    punct = string.punctuation
    words = [word for word in words if word not in punct and word != '``' and word != '\'\'' and word != '-']
    words_lower = [word.lower() for word in words]

    words_no_stop = [word for word in words_lower if word not in stop_words]
    lemmatized_words = [lemmatizer.lemmatize(word) for word in words_no_stop]
    return ' '.join(lemmatized_words)


@app.route('/file/<path:filename>')
def open_file(filename):
    try:
        name = filename[12:]

        fname = name[2].lower() + '/' + name
        fname = name[1].lower() + '/' + fname
        print(fname)
        return send_from_directory('D:/IS/data/en/a/', fname, as_attachment=False)
    except FileNotFoundError:
        return "Файл не найден", 404

@app.route('/search', methods=['POST'])
def search():
    search_query = request.form.get('search')
    search_option = request.form.get('search-option')
    api_url = 'http://localhost:8081'

    payload = {'query': normalize_text(search_query), 'option': search_option}
    headers = {'Content-Type': 'application/json'}

    response = requests.post(api_url, data=json.dumps(payload), headers=headers)

    if response.status_code == 200:
        results = response.json()
    else:
        results = ['Ошибка при получении данных от API. Попробуйте еще раз.']

    return jsonify(results)

if __name__ == '__main__':
    app.run(debug=True)
