# Desafio: Servidor na Nuvem 
Link do video:
https://youtu.be/kX0SEj9LQog?si=TOHRhDHWlpG3cF8u

## Descrição do Projeto

O projeto utiliza a Raspberry Pi Pico W (BitdogLab) para coletar dados de sensores como botão, joystick e temperatura interna. Esses dados são enviados continuamente para o ThingSpeak, um serviço de IoT baseado na nuvem.  
O objetivo é monitorar em tempo real variáveis como movimentação e temperatura, usando uma plataforma de fácil acesso remoto.

## Como Funciona

- **Joystick**: Mapeia as direções X e Y para entender o movimento feito.
- **Botão**: Confirma a ação do usuário.
- **Temperatura**: Lida do sensor interno da placa, enviada junto aos dados.
- **ThingSpeak**: Armazena e exibe os dados em tempo real via gráficos online.

### Comunicação com ThingSpeak

Os dados são enviados via protocolo TCP diretamente para os servidores da ThingSpeak.  
O endereço IP do servidor é obtido via DNS.  
A requisição HTTP é formatada e enviada com os dados dos sensores nos campos correspondentes.

## Desafios Enfrentados

Durante o desenvolvimento, configurar uma comunicação eficaz com o ThingSpeak trouxe desafios como:

- **Estabilidade da Conexão Wi-Fi**: A placa precisa conectar e manter comunicação com o roteador e a internet.
- **Formatação Correta da Requisição HTTP**: A URL precisa conter os campos e valores corretos para serem aceitos pelo ThingSpeak.
- **Resolução DNS e Conexão com o Servidor**: O nome do host (api.thingspeak.com) deve ser resolvido para um IP válido antes da conexão TCP.
- **Visualização dos Dados na Nuvem**: Após o envio, os dados são convertidos em gráficos acessíveis no painel da conta ThingSpeak.

## Benefícios do Sistema

- **Monitoramento à Distância**: Acompanhamento remoto dos dados enviados pelo dispositivo.
- **Dados Visuais em Nuvem**: Gráficos e histórico organizados pelo ThingSpeak.
- **Baixo Custo**: Baseado em microcontrolador acessível e componentes simples.
- **Expansível**: Possibilidade de integrar mais sensores e funcionalidades.
