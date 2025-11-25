// Lista de frutas (adicione mais se quiser)
const words = [
"abacate", "abacaxi", "abiu", "açaí", "acerola", "acelora-do-mato", "ackee", "african-cucumber",
"aguai", "agresta", "aguaviva", "alfarroba", "ameixa", "ameixa-amarela", "ameixa-de-natal",
"ameixa-japonesa", "ameixa-roxa", "amêndoa", "amora", "amora-branca", "amora-preta", "amora-vermelha",
"araticum", "araticum-do-brejo", "araucária-fruto", "araticum-cagão", "abiu-grande",
"ata", "ata-brava", "atemoia", "avelã",

"banana", "banana-maçã", "banana-nanica", "banana-prata", "banana-da-terra", "banana-ouro",
"bacaba", "bacabinha", "bacuri", "bacupari", "bambu-fruta", "baru", "bergamota",
"bergamota-murro", "biribá", "bilberry", "black-sapote", "blueberry", "boysenberry",
"buriti", "butiá", "butiá-do-cerrado",

"cabeludinha", "cacau", "cagaita", "cagaitinha", "cajá", "cajá-manga", "cajá-umbu", "caju",
"calabura", "camapu", "cambuci", "camu-camu", "caqui", "carambola", "carnaúba-fruto",
"castanha-portuguesa", "castanha-do-pará", "cereja", "cereja-do-rio-grande",
"cereja-de-pitanga", "cereja-europeia", "cereja-negra", "cereja-amarela",
"cirigüela", "coco", "coco-verde", "coco-anão",
"cranberry", "cupuaçu", "curuba", "curriola",

"damasco", "damasco-negro", "damasco-siberiano", "dekopon", "desert-lime",
"doce-de-abelha-fruto", "dragon-fruit", "durião",

"embaúba-fruto", "embaubarana", "embaúba-do-brejo", "embiriba",
"enset", "esfregadinha-fruto", "etrog",

"figo", "figo-da-índia", "figo-missioneiro", "framboesa", "framboesa-amarela",
"fruta-do-conde", "fruta-pão", "fruta-de-lobo", "fruta-de-ema", "fruta-de-sabiá",
"fruta-de-macaco", "fruta-de-tatu", "fruta-milagrosa", "fruta-pinha",

"gabi-fruto", "gabiroba", "gabiroba-do-campo", "gaiaku", "gamboa-fruto", "genipapo",
"goiaba", "goiaba-vermelha", "goiaba-branca", "golden-berry", "grapefruit", "graviola",
"groselha", "groselha-negra", "groselha-vermelha", "guabiroba", "guabiju", "guapuruvu-fruto",
"guaraná", "guarana-do-mato", "guavião",

"habañero-fruit-pepper", "hackberry", "halim-fruto", "hawthorn-berry", "honeyberry",
"hurmá (caqui oriental)",

"ilama", "ingá", "ingá-de-metro", "ingá-do-brejo", "ingá-mirim", "ixi-pixi",

"jabuticaba", "jabuticaba-sabará", "jabuticaba-branca", "jabuticaba-açu",
"jaca", "jaca-mole", "jaca-dura", "jambo", "jambo-vermelho", "jambo-amarelo",
"jamelão", "jenipapo", "jerivá-fruto", "jiripoca-fruto", "juá", "jujuba",
"juçara (fruto da palmeira)",

"kiwano", "kiwi", "kiwi-amarelo", "kumquat",

"laranja", "laranja-pera", "laranja-baía", "laranja-lima", "laranja-seleta",
"laranja-sanguínea", "lichia", "limão", "limão-siciliano", "limão-taiti", "limequat",
"longan", "loquat (nêspera)", "lulo", "lucuma",

"maçã", "maçã-verde", "maçã-gala", "maçã-fuji", "maçã-red-delicious", "maçã-brava",
"mamão", "mamão-formosa", "mamey", "mangaba", "manga", "manga-tommy", "manga-palmer",
"manga-haden", "manga-espada", "mangostão", "maracujá", "maracujá-doce", "maracujá-roxo",
"melancia", "melão", "melão-gália", "melão-orange", "melão-amarelo",
"mirtilo", "morango", "morango-silvestre", "murici", "murici-da-mata",
"mexerica", "mexerica-rio", "mexerica-pokan",

"nectarina",

"olho-de-boi-fruto", "olho-de-cabra-fruto", "olho-de-macaco-fruto",
"orangequat", "oranique",

"papaia", "pêra", "pêra-william", "pêra-portuguesa", "pessego", "pêssego", "pêssego-bravo",
"physalis", "pinha", "pinha-da-mata", "pimentão-fruto", "pineberry",
"pitanga", "pitanga-roxa", "pitanga-branca", "pitaya", "pitaya-branca",
"pitaya-vermelha", "pistache", "pixirica",
"plantain (banana-da-terra)",

"quina-do-mato-fruto",

"rambutan", "ramphal", "raspberry", "redcurrant", "romã", "rose-apple",

"salak (snake-fruit)", "sapoti", "sapota-branca", "sapucaia-fruto",
"sapucaia-peluda", "seriguela", "soursop", "tamarillo", "tamarindo",
"tâmara", "tangerina", "tangerina-cravo", "tangerina-murcote",
"tangerina-montenegro", "tangerina-pokan",
"tomate", "tomate-uva", "tomate-de-árvore",
"toranja", "tucumã",

"uva", "uva-verde", "uva-roxa", "uva-red-globe", "uva-sultana", "uva-moscato",
"uvaia", "uxi-amarelo", "uxi-do-cerrado",

"veludo-fruto", "veludo-da-mata", "velvet-apple",

"wampee", "wax-apple", "wolfberry (goji)",

"ximango-fruto",

"yuzu", "yangmei",

"zimbro-fruto"
]

const input = document.getElementById('fruitInput');
const box = document.getElementById('autoBox');
const w1 = document.getElementById('word1');
const w2 = document.getElementById('word2');
const w3 = document.getElementById('word3');

// ---------- TRIE (Árvore de prefixos) ----------
class TrieNode {
  constructor() {
    this.children = {};
    this.isEnd = false;
  }
}

class Trie {
  constructor() {
    this.root = new TrieNode();
  }

  insert(word) {
    let node = this.root;
    for (let char of word.toLowerCase()) {
      if (!node.children[char]) node.children[char] = new TrieNode();
      node = node.children[char];
    }
    node.isEnd = true;
  }

  search(prefix) {
    let node = this.root;
    for (let char of prefix.toLowerCase()) {
      if (!node.children[char]) return [];
      node = node.children[char];
    }
    return this.#collect(node, prefix.toLowerCase());
  }

  #collect(node, prefix) {
    let results = [];
    if (node.isEnd) results.push(prefix);
    for (let char in node.children) {
      results = results.concat(this.#collect(node.children[char], prefix + char));
    }
    return results;
  }
}

const trie = new Trie();
words.forEach(w => trie.insert(w));


// ---------- AUTOCOMPLETE ----------
input.addEventListener('input', () => {
  const val = input.value.trim();

  if (val === "") {
    box.classList.add("hidden");
    w1.textContent = "";
    w2.textContent = "";
    w3.textContent = "";
    return;
  }

  const results = trie.search(val).slice(0, 3);

  w1.textContent = results[0] || "";
  w2.textContent = results[1] || "";
  w3.textContent = results[2] || "";

  box.classList.remove("hidden");
});

// Preencher ao clicar
[w1, w2, w3].forEach(w => {
  w.addEventListener('click', () => {
    if (w.textContent.trim()) {
      input.value = w.textContent;
      box.classList.add("hidden");
    }
  });
});
