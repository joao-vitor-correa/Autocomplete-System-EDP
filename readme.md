Readme – Projeto autocomplete em C rodando em HTTP (Windows)

O projeto implementa um sistema de autocomplete em linguagem C, utilizando uma árvore AVL como estrutura principal para armazenar palavras e realizar buscas rápidas por prefixos.

O objetivo é demonstrar como estruturas de dados eficientes podem melhorar sistemas reais como buscas inteligentes, auto-preenchimento, sugestões de texto, etc.



Como compilar:

No Windows (MinGW), compile com:

gcc server.c autocomplete.c -o srv

Depois execute:

autocomplete.exe



Além do programa em C, criamos um servidor HTTP simples que responde a requisições no navegador, utilizando HTML para estruturar o servidor, Javascript para integrar o projeto em C com o frontend e Tailwind para estilização da página.



Rotas de API
GET /ping

Retorna:

{ "response": "pong" }
Usado para testar se o servidor está online.

GET /autocomplete?term=xxxx

GET /debbugin?term=xxxx

Rota especial de diagnóstico. Retorna:

o termo enviado

a quantidade de resultados encontrados

os resultados em um array

um trecho do request HTTP completo (até 1024 bytes)

Exemplo:
{
  "route": "/debbugin",
  "term": "ab",
  "qtd": 3,
  "resultados": ["abacate", "abacaxi", "abakashi"],
  "raw_request": "GET /debbugin?term=ab HTTP/1.1 ..."
}

Retorna as três melhores sugestões encontradas na árvore AVL com base no prefixo enviado no parâmetro term.
Exemplo de resposta:

{ "resultados": ["maca", "mamao", "manga"] }
