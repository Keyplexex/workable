FROM openjdk:21-jdk-slim
WORKDIR /app
COPY build.gradle .
COPY gradlew .
COPY gradle gradle
COPY src src
RUN chmod +x gradlew
RUN ./gradlew build -x test
CMD ["java", "-jar", "build/libs/carshow-1.0-SNAPSHOT.jar"]