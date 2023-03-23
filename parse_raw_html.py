from bs4 import BeautifulSoup
import os
from nltk.tokenize import word_tokenize, sent_tokenize
from nltk.corpus import stopwords
from nltk.stem import PorterStemmer, WordNetLemmatizer
import string
from tqdm import tqdm
import time

from collections import Counter
import matplotlib.pyplot as plt

lemmatizer = WordNetLemmatizer()
all_tokens = []

stop_words = set(stopwords.words("english"))

cnt_files = 0
cnt_words = 0

def get_visible_text(soup):
    # Извлечение названия статьи
    title = soup.find("h1", {"class": "firstHeading"}).text

    # Извлечение первого абзаца
    content = soup.find("div", {"class": "mw-parser-output"}) or soup.body
    first_paragraph = None
    other_text = ""
    for el in content.find_all(["p", "table"]):
        if el.name == "p" and el.text.strip() != "":
            first_paragraph = el.text
            break

    for p in content.find_all("p")[1:]:
        other_text += p.text
    for p in content.find_all(['ul', "il"]):
        other_text += p.text
    return (title, first_paragraph, other_text)

def normalize_text(text):
    words = word_tokenize(text)
    
    punct = string.punctuation
    words = [word for word in words if word not in punct and word != '``' and word != '\'\'' and word != '-']
    words_lower = [word.lower() for word in words] 

    words_no_stop = [word for word in words_lower if word not in stop_words]
    lemmatized_words = [lemmatizer.lemmatize(word) for word in words_no_stop]

    global cnt_words
    cnt_words += len(lemmatized_words)
    # all_tokens.extend(lemmatized_words)
    return lemmatized_words

def process_file(file_path):
    try:
        # открываем файл
        with open(file_path, 'r', encoding='utf-8') as f:
            html_content = f.read()

        soup = BeautifulSoup(html_content, "html.parser")

        # Является ли страница перенаправлением
        redirect = soup.find("meta", {"http-equiv": "Refresh"})
        if redirect:
            return
        
        (title, first_paragraph, other_text) = get_visible_text(soup)

        title = normalize_text(title)
        first_paragraph = normalize_text(first_paragraph)
        other_text = normalize_text(other_text)

        with open(os.path.join(res_folder, os.path.basename(file_path)), mode='w', encoding="utf-8") as f:
            f.write(' '.join(title) + '\n')
            f.write(' '.join(first_paragraph) + '\n')
            f.write(' '.join(other_text))

        global cnt_files
        cnt_files += 1

        if cnt_files % 1000 == 0:
            print(cnt_files)
    except Exception as e:
        return

def process_folder(folder_path, res_folder):
    global cnt_files
    for root, dirs, files in os.walk(folder_path):
        for file in files:
            if not str(os.path.basename(file)).startswith('A') and not str(os.path.basename(file)).startswith('a'):
                file_path = os.path.join(root, file)
                os.remove(file_path)
                continue

            file_path = os.path.join(root, file)
            process_file(file_path)

folder_path = os.path.join('data', 'en', 'a')
res_folder = os.path.join('texts', 'en')
start = time.time()
process_folder(folder_path, res_folder) 
end = time.time()
print('Time: ', int(end - start) // 60, 'm ', int(end - start) % 60, 's')
print('Words: ', cnt_words)
print('Files: ', cnt_files)

# построение графика закона Ципфа
# freq = Counter(all_tokens)
# most_common_words = freq.most_common()
# rank = range(1, len(most_common_words)+1)
# freqs = [f for (w, f) in most_common_words]
# plt.plot(rank, freqs)
# plt.xlabel('Rank')
# plt.ylabel('Freq')
# plt.show()